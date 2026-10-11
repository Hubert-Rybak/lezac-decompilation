"""Execute the original RLE and its real stack guard, without instruction patches.

These probes intentionally reproduce the original unsafe memory accesses inside
Unicorn's sandbox. They do not implement that behavior in the safe asset parser.
"""
import hashlib
import struct
from pathlib import Path
import pytest
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE, UC_HOOK_CODE, UC_PROT_READ, UC_PROT_EXEC
from unicorn.x86_const import UC_X86_REG_CS, UC_X86_REG_IP, UC_X86_REG_SS, UC_X86_REG_SP, UC_X86_REG_DS

ROOT = Path(__file__).resolve().parents[1]
SHA256 = '7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec'


@pytest.fixture(scope='module')
def exe():
    data = (ROOT / 'original/game/LEZAC.EXE').read_bytes()
    assert hashlib.sha256(data).hexdigest() == SHA256
    return data


def original_rle(exe, payload, required, tail_pattern):
    header = struct.unpack_from('<14H', exe)
    header_bytes, relocations, table = header[4] * 16, header[3], header[12]
    image = bytearray(exe[header_bytes:])
    for i in range(relocations):
        offset, segment = struct.unpack_from('<HH', exe, table + 4*i)
        location = segment * 16 + offset
        word = struct.unpack_from('<H', image, location)[0]
        struct.pack_into('<H', image, location, (word + 0x1000) & 0xffff)
    uc = Uc(UC_ARCH_X86, UC_MODE_16)
    uc.mem_map(0, 0x100000)
    uc.mem_write(0x10000, bytes(image))
    uc.mem_protect(0x10000, (len(image) + 4095) & ~4095, UC_PROT_READ | UC_PROT_EXEC)
    uc.mem_write(0x30000, payload + bytes([tail_pattern]) * 1024)
    uc.mem_write(0x40000, b'\xcd' * (required + 64))
    uc.mem_write(0x60000, struct.pack('<4H', 0, 0x3000, 0, 0x4000))
    # Far return, output length, reference to destination farptr,
    # input length, reference to source farptr. DS:1afa guard remains zero.
    uc.mem_write(0x7ff00, struct.pack('<8H', 0xff00, 0x1000, required, 4, 0x6000, len(payload), 0, 0x6000))
    for reg, value in [(UC_X86_REG_CS,0x182d),(UC_X86_REG_IP,0),
                       (UC_X86_REG_SS,0x7000),(UC_X86_REG_SP,0xff00),(UC_X86_REG_DS,0x6000)]:
        uc.reg_write(reg,value)
    reads, writes, stack_checks = [], [], []
    def on_stack_check(machine, address, size, user):
        stack_checks.append(address)
    def on_read(machine, access, address, size, value, user):
        if 0x30000 <= address < 0x30000 + len(payload) + 1024:
            reads.append(address - 0x30000)
    def on_write(machine, access, address, size, value, user):
        if 0x40000 <= address < 0x40000 + required + 64:
            writes.append(address - 0x40000)
    # Global memory hooks interfered with RETF in this Unicorn setup. Bound
    # observation to the buffers of interest, leaving stack/control flow alone.
    uc.hook_add(UC_HOOK_MEM_READ, on_read, begin=0x30000, end=0x3ffff)
    uc.hook_add(UC_HOOK_MEM_WRITE, on_write, begin=0x40000, end=0x4ffff)
    uc.hook_add(UC_HOOK_CODE, on_stack_check, begin=0x196df, end=0x196df)
    uc.emu_start(0x182d0, 0x1ff00, count=5_000_000)
    assert uc.reg_read(UC_X86_REG_CS) == 0x1000
    assert uc.reg_read(UC_X86_REG_IP) == 0xff00
    assert uc.reg_read(UC_X86_REG_SP) == 0xff10
    assert stack_checks == [0x196df]
    assert bytes(uc.mem_read(0x10000, len(image))) == bytes(image)
    return bytes(uc.mem_read(0x40000, required + 32)), max(reads), max(writes)


def plane(exe, index, plane_index):
    level = struct.unpack_from('<7I', exe, 0xcc1a)[index]
    data = (ROOT / 'original/game/LIVELS.SCH').read_bytes()
    assert hashlib.sha256(data).hexdigest() == 'd8b553871014108515d6af05b0853de8ccc22e4d3e44d63533ba1167fdc8c7b2'
    width, height = struct.unpack_from('<HH', data, level)
    assert plane_index in (0, 1)
    cursor = level + 8
    payload = b''
    for p in range(plane_index + 1):
        size = struct.unpack_from('<H', data, cursor)[0]
        cursor += 2
        payload = data[cursor:cursor + size]
        cursor += size
    prefix = bytearray()
    for control, first, second in struct.iter_unpack('BBB', payload):
        prefix.extend(bytes([first]) * ((control >> 4) + 1))
        prefix.extend(bytes([second]) * ((control & 15) + 1))
    return payload, width * height * (plane_index + 1), bytes(prefix)


@pytest.mark.parametrize('index', range(7))
@pytest.mark.parametrize('plane_index', range(2))
@pytest.mark.parametrize('tail', [0, 0x55])
def test_real_plane_prefix_and_unknown_memory_tail(exe, index, plane_index, tail):
    payload, required, prefix = plane(exe, index, plane_index)
    expected_missing = [(9,12),(25,5),(11,11),(9,15),(13,30),(9,22),(9,12)]
    assert required - len(prefix) == expected_missing[index][plane_index]
    output, max_read, max_write = original_rle(exe, payload, required, tail)
    assert output[:len(prefix)] == prefix
    assert set(output[len(prefix):required]) == {tail}
    assert max_read >= len(payload)
    assert max_write >= required


def test_complete_synthetic_payload_still_overwrites_one_byte(exe):
    output, max_read, max_write = original_rle(exe, bytes.fromhex('12 41 42'), 5, 0)
    assert output[:6] == b'AABBBB'
    assert max_read == 2
    assert max_write == 5
