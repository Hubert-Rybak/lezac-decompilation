"""Call routines of the ORIGINAL executable's runtime segments under Unicorn.

The relocated load image is mapped read/execute only; arguments are passed in
registers or on the stack exactly as the original far call sites do. Runtime
errors are observed at the original RunError entry, not stubbed.
"""
import hashlib
import struct
from pathlib import Path

from unicorn import Uc, UC_ARCH_X86, UC_MODE_16, UC_HOOK_CODE, UC_PROT_READ, UC_PROT_EXEC
from unicorn.x86_const import (
    UC_X86_REG_AX, UC_X86_REG_BX, UC_X86_REG_CX, UC_X86_REG_DX,
    UC_X86_REG_SI, UC_X86_REG_DI, UC_X86_REG_BP, UC_X86_REG_SP,
    UC_X86_REG_CS, UC_X86_REG_DS, UC_X86_REG_ES, UC_X86_REG_SS,
    UC_X86_REG_IP, UC_X86_REG_EFLAGS,
)

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / 'original/game/LEZAC.EXE'
SHA256 = '7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec'
LOAD_SEG = 0x1000
RUNTIME_SEG = 0x1920   # System unit code segment at this load segment
DATA_SEG = 0x1AA2      # relocated DS
RUN_ERROR = 0x192E2    # RunError entry (error code in AX)
STACK_SEG = 0x8000
SENTINEL_SEG, SENTINEL_IP = 0x9000, 0x0000


def relocated_image():
    data = EXE.read_bytes()
    assert hashlib.sha256(data).hexdigest() == SHA256
    header = struct.unpack_from('<14H', data)
    image = bytearray(data[header[4] * 16:])
    for i in range(header[3]):
        offset, segment = struct.unpack_from('<HH', data, header[12] + 4 * i)
        position = segment * 16 + offset
        word = struct.unpack_from('<H', image, position)[0]
        struct.pack_into('<H', image, position, (word + LOAD_SEG) & 0xFFFF)
    return bytes(image)


class OriginalRuntime:
    def __init__(self):
        self.image = relocated_image()
        uc = Uc(UC_ARCH_X86, UC_MODE_16)
        uc.mem_map(0, 0x100000)
        uc.mem_write(LOAD_SEG * 16, self.image)
        size = (len(self.image) + 4095) & ~4095
        # Data segment must stay writable (RandSeed); protect the code pages only.
        code_end = (DATA_SEG * 16 - LOAD_SEG * 16) & ~4095
        uc.mem_protect(LOAD_SEG * 16, code_end, UC_PROT_READ | UC_PROT_EXEC)
        self.uc = uc
        self.error = None
        uc.hook_add(UC_HOOK_CODE, self._on_error, begin=RUN_ERROR, end=RUN_ERROR)
        self.code_hash = hashlib.sha256(bytes(uc.mem_read(LOAD_SEG * 16, code_end))).hexdigest()
        self._code_end = code_end

    def _on_error(self, uc, address, size, user):
        self.error = uc.reg_read(UC_X86_REG_AX)
        uc.emu_stop()

    def far_call(self, offset, regs=None, stack_words=(), limit=200_000):
        """Far call RUNTIME_SEG:offset; returns (registers, flags) or raises on RunError."""
        uc = self.uc
        self.error = None
        regs = dict(regs or {})
        for name, reg in (('ax', UC_X86_REG_AX), ('bx', UC_X86_REG_BX), ('cx', UC_X86_REG_CX),
                          ('dx', UC_X86_REG_DX), ('si', UC_X86_REG_SI), ('di', UC_X86_REG_DI)):
            uc.reg_write(reg, regs.get(name, 0) & 0xFFFF)
        uc.reg_write(UC_X86_REG_DS, DATA_SEG)
        uc.reg_write(UC_X86_REG_ES, DATA_SEG)
        uc.reg_write(UC_X86_REG_SS, STACK_SEG)
        uc.reg_write(UC_X86_REG_BP, 0)
        words = [SENTINEL_IP, SENTINEL_SEG] + [w & 0xFFFF for w in reversed(stack_words)]
        # stack_words are given in push order; the callee sees the last pushed nearest.
        sp = 0xF000 - 2 * len(words)
        uc.mem_write(STACK_SEG * 16 + sp, struct.pack('<%dH' % len(words), *words))
        uc.reg_write(UC_X86_REG_SP, sp)
        uc.reg_write(UC_X86_REG_EFLAGS, 0x0002)
        uc.reg_write(UC_X86_REG_CS, RUNTIME_SEG)
        uc.reg_write(UC_X86_REG_IP, offset)
        uc.emu_start(RUNTIME_SEG * 16 + offset, SENTINEL_SEG * 16 + SENTINEL_IP, count=limit)
        if self.error is not None:
            return None, self.error
        assert uc.reg_read(UC_X86_REG_CS) == SENTINEL_SEG and uc.reg_read(UC_X86_REG_IP) == SENTINEL_IP
        out = {name: uc.reg_read(reg) for name, reg in (
            ('ax', UC_X86_REG_AX), ('bx', UC_X86_REG_BX), ('cx', UC_X86_REG_CX),
            ('dx', UC_X86_REG_DX), ('si', UC_X86_REG_SI), ('di', UC_X86_REG_DI))}
        out['flags'] = uc.reg_read(UC_X86_REG_EFLAGS)
        return out, None

    def read_data(self, offset, length):
        return bytes(self.uc.mem_read(DATA_SEG * 16 + offset, length))

    def write_data(self, offset, data):
        self.uc.mem_write(DATA_SEG * 16 + offset, bytes(data))

    def verify_unmodified(self):
        current = hashlib.sha256(bytes(self.uc.mem_read(LOAD_SEG * 16, self._code_end))).hexdigest()
        assert current == self.code_hash
        assert hashlib.sha256(EXE.read_bytes()).hexdigest() == SHA256
