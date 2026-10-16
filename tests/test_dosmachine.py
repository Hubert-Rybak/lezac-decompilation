"""The deterministic DOS machine model (tools/dosmachine.py) used for lockstep runs.

Small hand-assembled MZ programs exercise the DOS and BIOS services the game
and the Free Pascal runtime use; a determinism check runs the authenticated
original twice with the same input trace.
"""
import json
import struct
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import dosmachine  # noqa: E402
from dosmachine import Machine, MachineError, TICK_MS  # noqa: E402


class Asm:
    """Just enough of an 8086 assembler for test programs (labels are patched)."""

    def __init__(self):
        self.code = bytearray()
        self.labels = {}
        self.fixups = []

    def emit(self, *data):
        for item in data:
            self.code += bytes(item) if isinstance(item, (bytes, list)) else bytes([item])
        return self

    def label(self, name):
        self.labels[name] = len(self.code)
        return self

    def imm16_label(self, name):
        self.fixups.append(('abs', len(self.code), name))
        self.code += b'\0\0'
        return self

    def jcc(self, opcode, name):
        self.code += bytes([opcode, 0])
        self.fixups.append(('rel8', len(self.code) - 1, name))
        return self

    def mz(self):
        for kind, at, name in self.fixups:
            target = self.labels[name]
            if kind == 'abs':
                struct.pack_into('<H', self.code, at, target)
            else:
                self.code[at] = (target - (at + 1)) & 0xFF
        header = bytearray(32)
        total = 32 + len(self.code)
        struct.pack_into('<14H', header, 0, 0x5A4D, total % 512, (total + 511) // 512, 0, 2,
                         0x10, 0xFFFF, 0x10, 0x100, 0, 0, 0, 0x1C, 0)
        return bytes(header) + bytes(self.code)


NO_DELAY = (0x7000, 0)        # no Crt.Delay in these programs


def run(program, files=None, events=None, steps=5):
    machine = Machine(program, files or {}, NO_DELAY, label='test')
    if events:
        machine.schedule(events)
    for _ in range(steps):
        event = machine.step()
        if event[0] in ('exit', 'stalled'):
            return machine, event
    return machine, event


def exit_with_al(asm):
    return asm.emit(0xB4, 0x4C, 0xCD, 0x21)        # mov ah,4Ch ; int 21h


def test_file_open_seek_read_and_exit_code():
    asm = Asm()
    asm.emit(0x0E, 0x1F)                                  # push cs ; pop ds
    asm.emit(0xBA).imm16_label('name').emit(0xB8, 0x00, 0x3D, 0xCD, 0x21)   # open
    asm.jcc(0x72, 'fail')
    asm.emit(0x89, 0xC3)                                  # mov bx,ax
    asm.emit(0xB8, 0x00, 0x42, 0x31, 0xC9, 0xBA, 0x02, 0x00, 0xCD, 0x21)   # seek 2
    asm.emit(0xBA).imm16_label('buf').emit(0xB9, 0x01, 0x00, 0xB4, 0x3F, 0xCD, 0x21)  # read 1
    asm.emit(0xB4, 0x3E, 0xCD, 0x21)                      # close
    asm.emit(0xA0).imm16_label('buf')                     # mov al,[buf]
    exit_with_al(asm)
    asm.label('fail').emit(0xB0, 0xFF)
    exit_with_al(asm)
    asm.label('name').emit(b'C:\\GAME\\TEST.DAT\0')
    asm.label('buf').emit(0)
    program = asm.mz()
    machine, event = run(program, {'test.dat': b'\x01\x02\x7b\x04'})
    assert event == ('exit', 0x7B)
    machine, event = run(program, {})
    assert event == ('exit', 0xFF)


def test_program_block_has_a_memory_control_block():
    asm = Asm()
    asm.emit(0xB0, 0x00)
    exit_with_al(asm)
    machine = Machine(asm.mz(), {}, NO_DELAY)
    mcb = machine.read((machine.psp - 1) * 16, 5)
    assert mcb[0:1] == b'Z'
    assert struct.unpack('<HH', mcb[1:5]) == (machine.psp, dosmachine.TOP_SEG - machine.psp)
    assert machine.word(machine.psp * 16 + 2) == dosmachine.TOP_SEG


def test_blocking_key_read_waits_for_the_scheduled_key_in_virtual_time():
    asm = Asm()
    asm.emit(0xB4, 0x00, 0xCD, 0x16)                      # int 16h, AH=0
    asm.emit(0x88, 0xE0)                                  # mov al,ah (scan code)
    exit_with_al(asm)
    events = [{'t_ms': 1000.0, 'down': 'space'}, {'t_ms': 1100.0, 'up': 'space'}]
    machine, event = run(asm.mz(), events=events)
    assert event == ('exit', 0x39)
    assert machine.vtime >= 1000.0
    # timer interrupts were delivered for the waited time, nothing more
    assert machine.ticks == int(1000.0 // TICK_MS)


def test_dos_time_follows_virtual_time_only():
    asm = Asm()
    asm.emit(0xB4, 0x2C, 0xCD, 0x21)                      # get time
    asm.emit(0x88, 0xD0)                                  # mov al,dl (hundredths)
    exit_with_al(asm)
    machine, event = run(asm.mz())
    assert event == ('exit', 0) and machine.dos_time() == (12, 0, 0, 0)
    machine.ticks = 91                                    # 91 ticks = 4.998 s
    assert machine.dos_time() == (12, 0, 4, 99)


def test_unknown_services_fail_closed():
    asm = Asm()
    asm.emit(0xB4, 0x99, 0xCD, 0x21)
    exit_with_al(asm)
    with pytest.raises(MachineError, match='INT 21h AX=99'):
        run(asm.mz())


def test_default_palette_is_the_vga_bios_table():
    palette = dosmachine.default_palette()
    assert len(palette) == 768
    assert list(palette[0:3]) == [0, 0, 0] and list(palette[15 * 3:16 * 3]) == [63, 63, 63]
    assert list(palette[32 * 3:33 * 3]) == [0, 0, 63] and list(palette[247 * 3:248 * 3]) == [11, 12, 16]


def test_original_runs_identically_twice():
    original = ROOT / 'original/game/LEZAC.EXE'
    if not original.is_file():
        pytest.skip('original/game not imported (tools/fetch_original.py)')
    import lockstep
    trace = json.loads((ROOT / 'emulator/traces/boot-smoke.json').read_text())
    report = lockstep.run_lockstep(None, trace, 300, quiet=True)
    assert report['divergence'] is None
    assert report['steps'] == 300
