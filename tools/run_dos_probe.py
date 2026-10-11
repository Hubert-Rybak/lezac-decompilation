#!/usr/bin/env python3
"""Run a computational MZ probe in Unicorn with a limited DOS service fixture.

This is NOT a complete DOS emulator or a gameplay validation environment. Guest
instructions execute normally, but only the documented INT 21h services below
are provided. No guest filesystem, network, hardware or host process API exists.
Unknown services and instruction-budget exhaustion fail closed.
"""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16, UC_HOOK_INTR
from unicorn.x86_const import (
    UC_X86_REG_AX, UC_X86_REG_BX, UC_X86_REG_CX, UC_X86_REG_DX,
    UC_X86_REG_CS, UC_X86_REG_IP, UC_X86_REG_SS, UC_X86_REG_SP,
    UC_X86_REG_DS, UC_X86_REG_ES, UC_X86_REG_EFLAGS,
)


def run_mz(data, instruction_limit=5_000_000):
    if not isinstance(instruction_limit, int) or not 1 <= instruction_limit <= 50_000_000:
        raise ValueError('Instruction limit must be in 1..50000000')
    if len(data) < 28 or data[:2] != b'MZ':
        raise ValueError('Expected an MZ executable')
    h = struct.unpack_from('<14H', data)
    header_bytes = h[4] * 16
    end = (h[2] - 1) * 512 + (h[1] or 512)
    if h[2] == 0 or h[1] > 511 or not 28 <= header_bytes < end <= len(data):
        raise ValueError('Invalid MZ header or module bounds')
    if h[12] < 28 or h[12] + 4 * h[3] > header_bytes:
        raise ValueError('Invalid relocation table')
    load_segment = 0x1000
    psp_segment = load_segment - 0x10
    image = bytearray(data[header_bytes:end])
    if len(image) > 0x90000 or h[11] * 16 + h[10] >= len(image):
        raise ValueError('Module or entry outside fixture conventional memory')
    if (load_segment + h[7]) * 16 + h[8] >= 0xa0000:
        raise ValueError('Stack outside fixture conventional memory')
    for i in range(h[3]):
        offset, segment = struct.unpack_from('<HH', data, h[12] + i * 4)
        at = segment * 16 + offset
        if at + 2 > len(image):
            raise ValueError('Relocation outside initialized module')
        value = struct.unpack_from('<H', image, at)[0]
        struct.pack_into('<H', image, at, (value + load_segment) & 0xffff)
    machine = Uc(UC_ARCH_X86, UC_MODE_16)
    machine.mem_map(0, 0x200000)
    machine.mem_write(load_segment * 16, bytes(image))
    machine.mem_write(psp_segment * 16, b'\xcd\x20')
    machine.mem_write(psp_segment * 16 + 2, struct.pack('<H', 0x9fff))
    machine.mem_write(psp_segment * 16 + 0x2c, struct.pack('<H', 0xe00))
    machine.mem_write(0xe000, b'\0\0\x01\x00C:\\PROBE.EXE\0')
    machine.mem_write(psp_segment * 16 + 0x80, b'\x00\x0d')
    for register, value in ((UC_X86_REG_CS, load_segment + h[11]),
                            (UC_X86_REG_IP, h[10]),
                            (UC_X86_REG_SS, load_segment + h[7]),
                            (UC_X86_REG_SP, h[8]),
                            (UC_X86_REG_DS, psp_segment),
                            (UC_X86_REG_ES, psp_segment), (UC_X86_REG_EFLAGS, 2)):
        machine.reg_write(register, value)
    stdout, stderr, calls = bytearray(), bytearray(), Counter()
    exit_code = None

    def interrupt(uc, number, user):
        nonlocal exit_code
        ax = uc.reg_read(UC_X86_REG_AX)
        ah, al = ax >> 8, ax & 255
        bx, cx, dx = (uc.reg_read(r) for r in (UC_X86_REG_BX, UC_X86_REG_CX, UC_X86_REG_DX))
        ds = uc.reg_read(UC_X86_REG_DS)
        if number != 0x21:
            raise NotImplementedError(f'Unsupported interrupt {number:#x}, AX={ax:04x}')
        calls[f'{ah:02x}'] += 1
        flags = uc.reg_read(UC_X86_REG_EFLAGS) & ~1
        uc.reg_write(UC_X86_REG_EFLAGS, flags)
        if ah == 0x30:  # Fixed DOS 5.0 fixture; not an observed host OS version.
            uc.reg_write(UC_X86_REG_AX, 5)
            uc.reg_write(UC_X86_REG_BX, 0)
            uc.reg_write(UC_X86_REG_CX, 0)
        elif ah == 0x35:
            offset, segment = struct.unpack('<HH', bytes(uc.mem_read(al * 4, 4)))
            uc.reg_write(UC_X86_REG_BX, offset)
            uc.reg_write(UC_X86_REG_ES, segment)
        elif ah == 0x25:
            uc.mem_write(al * 4, struct.pack('<HH', dx, ds))
        elif ah == 0x44 and al == 0 and bx in (0, 1, 2):
            uc.reg_write(UC_X86_REG_DX, 0x80)  # Standard handles are character devices.
        elif ah == 0x71:  # DOS 5 fixture has no Win9x long-filename services.
            uc.reg_write(UC_X86_REG_AX, 0x7100)
            uc.reg_write(UC_X86_REG_EFLAGS, flags | 1)
        elif ah == 0x40 and bx in (1, 2):
            output = stdout if bx == 1 else stderr
            if len(stdout) + len(stderr) + cx > 1_048_576:
                raise RuntimeError('Guest output budget exceeded')
            output.extend(uc.mem_read(ds * 16 + dx, cx))
            uc.reg_write(UC_X86_REG_AX, cx)
        elif ah == 0x4c:
            exit_code = al
            uc.emu_stop()
        else:
            raise NotImplementedError(f'Unsupported INT 21h service AX={ax:04x}, BX={bx:04x}')

    machine.hook_add(UC_HOOK_INTR, interrupt)
    entry = (load_segment + h[11]) * 16 + h[10]
    machine.emu_start(entry, 0, count=instruction_limit)
    if exit_code is None:
        raise RuntimeError('Guest did not terminate within the instruction budget')
    return {'target_sha256': hashlib.sha256(data).hexdigest(),
            'format': 'dos-mz', 'exit_code': exit_code,
            'stdout': stdout.decode('cp437'), 'stderr': stderr.decode('cp437'),
            'dos_functions': dict(sorted(calls.items())),
            'instruction_limit': instruction_limit,
            'environment': 'Unicorn x86-16 plus limited synthetic DOS 5 service fixture'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--instruction-limit', type=int, default=5_000_000)
    args = parser.parse_args()
    result = run_mz(args.executable.read_bytes(), args.instruction_limit)
    print(json.dumps(result, indent=2))
    return result['exit_code']


if __name__ == '__main__':
    raise SystemExit(main())
