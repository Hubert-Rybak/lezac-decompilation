"""Execute the original MZ load image; no patched instructions or stubbed calls."""
from collections import Counter
import hashlib
from pathlib import Path
import struct

from unicorn import Uc, UC_ARCH_X86, UC_MODE_16, UC_HOOK_CODE, UC_HOOK_INTR, UC_PROT_READ, UC_PROT_EXEC
from unicorn.x86_const import (
    UC_X86_REG_AX, UC_X86_REG_BX, UC_X86_REG_CX, UC_X86_REG_DX,
    UC_X86_REG_SI, UC_X86_REG_DI, UC_X86_REG_BP, UC_X86_REG_SP,
    UC_X86_REG_CS, UC_X86_REG_DS, UC_X86_REG_ES, UC_X86_REG_SS,
    UC_X86_REG_IP, UC_X86_REG_EFLAGS,
)

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "original/game/LEZAC.EXE"
SHA256 = "7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec"


class DosMachine:
    LOAD_SEG = 0x1000
    LOAD = LOAD_SEG << 4
    DS = 0x3000
    SS = 0x8000
    SP = 0xF000
    BP = 0xE800
    STOP_IP = 0xFF00
    STOP = LOAD + STOP_IP

    def __init__(self):
        original = EXE.read_bytes()
        assert hashlib.sha256(original).hexdigest() == SHA256
        fields = struct.unpack_from("<14H", original)
        magic, last, pages, count, paragraphs = fields[:5]
        assert magic == 0x5A4D
        image_end = (pages - 1) * 512 + (last or 512)
        self.header_size = paragraphs * 16
        assert image_end == len(original) == 52384
        image = bytearray(original[self.header_size:image_end])
        self.relocations = []
        for i in range(count):
            offset, segment = struct.unpack_from("<HH", original, fields[12] + 4 * i)
            position = segment * 16 + offset
            assert 0 <= position <= len(image) - 2
            word = struct.unpack_from("<H", image, position)[0]
            struct.pack_into("<H", image, position, (word + self.LOAD_SEG) & 0xFFFF)
            self.relocations.append(position)
        assert self.header_size == 1904 and len(self.relocations) == 468
        self.image = bytes(image)
        self.uc = Uc(UC_ARCH_X86, UC_MODE_16)
        self.uc.mem_map(0, 2 * 1024 * 1024)
        self.uc.mem_write(self.LOAD, self.image)
        # Guest instructions cannot alter any page containing the load image.
        protected_size = (len(image) + 4095) & ~4095
        self.uc.mem_protect(self.LOAD, protected_size, UC_PROT_READ | UC_PROT_EXEC)
        self.stack_checks = 0
        self.calls = Counter()
        self.uc.hook_add(UC_HOOK_CODE, self._stack_check, begin=0x196DF, end=0x196DF)
        self.uc.hook_add(UC_HOOK_INTR, self._interrupt)
        self.write_word(self.DS, 0x1AFA, 0)

    def _stack_check(self, uc, address, size, data):
        self.stack_checks += 1

    @staticmethod
    def _interrupt(uc, number, data):
        raise AssertionError(f"Unexpected interrupt {number:#x}; nothing is stubbed")

    def write(self, segment, offset, data):
        self.uc.mem_write(segment * 16 + offset, bytes(data))

    def read(self, segment, offset, length):
        return bytes(self.uc.mem_read(segment * 16 + offset, length))

    def write_word(self, segment, offset, value):
        self.write(segment, offset, struct.pack("<H", value & 0xFFFF))

    def read_word(self, segment, offset):
        return struct.unpack("<H", self.read(segment, offset, 2))[0]

    def run(self, entry, arguments, instruction_limit=2_000_000):
        """Arguments are words at BP+4, BP+6, ... after PUSH BP."""
        uc = self.uc
        for register in (UC_X86_REG_AX, UC_X86_REG_BX, UC_X86_REG_CX,
                         UC_X86_REG_DX, UC_X86_REG_SI, UC_X86_REG_DI):
            uc.reg_write(register, 0xA55A)
        for register, value in ((UC_X86_REG_CS, self.LOAD_SEG),
                                (UC_X86_REG_DS, self.DS),
                                (UC_X86_REG_ES, 0x4000),
                                (UC_X86_REG_SS, self.SS),
                                (UC_X86_REG_SP, self.SP),
                                (UC_X86_REG_BP, self.BP),
                                (UC_X86_REG_EFLAGS, 2)):
            uc.reg_write(register, value)
        words = [self.STOP_IP, *arguments]
        self.write(self.SS, self.SP, struct.pack("<" + "H" * len(words), *words))
        checks_before = self.stack_checks
        uc.emu_start(entry, self.STOP, count=instruction_limit)
        assert uc.reg_read(UC_X86_REG_CS) == self.LOAD_SEG
        assert uc.reg_read(UC_X86_REG_IP) == self.STOP_IP, "Failed to return to sentinel"
        assert uc.reg_read(UC_X86_REG_SP) == self.SP + 2 + 2 * len(arguments)
        assert uc.reg_read(UC_X86_REG_BP) == self.BP
        assert self.stack_checks == checks_before + 1, "Original stack check not executed"
        self.calls[entry] += 1

    def verify_unmodified(self):
        assert self.read(self.LOAD_SEG, 0, len(self.image)) == self.image
        assert hashlib.sha256(EXE.read_bytes()).hexdigest() == SHA256
