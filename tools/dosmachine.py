#!/usr/bin/env python3
"""Deterministic PC/DOS machine model for running LEZAC.EXE builds under Unicorn.

Guest instructions (the program, its runtime and units) execute unchanged.
The machine supplies, in Python, only what a PC BIOS and DOS would: interrupt
services (INT 10h video, 16h keyboard, 21h DOS), the timer and keyboard
hardware interrupts, VGA DAC/status ports, the PC speaker ports and a file
system holding copies of the game files.

Time is VIRTUAL and advances only when the program calls Crt.Delay (the call
is recognised at the Delay entry address of the build and completed by the
machine without executing its busy loop) or when the program waits for input
(blocking INT 16h read, or repeated empty INT 16h polls). Timer interrupts
(18.2 Hz) and key events are delivered at the virtual times at which they
fall, between program instructions, by running the installed interrupt
handlers to completion. Two builds that behave identically therefore see
identical inputs, times and interrupt sequences. Instruction timing is not
modelled: the game's state does not depend on it (pacing goes through Delay
and DOS GetTime). This is a test fixture, not a general DOS emulator; unknown
services fail closed.
"""
import fnmatch
import struct

from unicorn import (Uc, UcError, UC_ARCH_X86, UC_MODE_16, UC_HOOK_CODE, UC_HOOK_INTR,
                     UC_HOOK_INSN, UC_HOOK_MEM_READ)
from unicorn.x86_const import (
    UC_X86_REG_AX, UC_X86_REG_BX, UC_X86_REG_CX, UC_X86_REG_DX,
    UC_X86_REG_SI, UC_X86_REG_DI, UC_X86_REG_BP, UC_X86_REG_SP,
    UC_X86_REG_CS, UC_X86_REG_DS, UC_X86_REG_ES, UC_X86_REG_SS,
    UC_X86_REG_IP, UC_X86_REG_EFLAGS, UC_X86_INS_IN, UC_X86_INS_OUT,
)

PIT_HZ = 1193182
TICK_MS = 65536 * 1000 / PIT_HZ          # 54.9254 ms between timer interrupts
BIOS_SEG = 0xF000
STUB_OFS = 0x0100                       # F000:0100 + 4*n: per-vector BIOS entry
SENTINEL = (BIOS_SEG, 0xFFF0)           # return address for machine-run handlers
ENV_SEG = 0x0F00
TOP_SEG = 0xA000
VIDEO = 0xA0000
HLE_TICK, HLE_KEY = 0xF0, 0xF1          # private vectors used by the BIOS stubs
POLL_LIMIT = 200                        # empty INT 16h polls before idling

# Scan code set 1 make codes and the BIOS buffer word for the trace key names.
KEYS = {
    'z': (0x2C, ord('z')), 'x': (0x2D, ord('x')), 'c': (0x2E, ord('c')),
    'm': (0x32, ord('m')), 'n': (0x31, ord('n')), 'Left': (0x4B, 0), 'Right': (0x4D, 0),
    'Up': (0x48, 0), 'Down': (0x50, 0), '0': (0x0B, ord('0')), 'KP_0': (0x52, 0),
    'KP_Insert': (0x52, 0), '1': (0x02, ord('1')), '2': (0x03, ord('2')),
    'i': (0x17, ord('i')), 'l': (0x26, ord('l')), 'r': (0x13, ord('r')),
    'e': (0x12, ord('e')), 's': (0x1F, ord('s')), 'space': (0x39, 0x20),
    'Return': (0x1C, 0x0D), 'Escape': (0x01, 0x1B),
}
MAKE_TO_ASCII = {make: ascii_ for make, ascii_ in KEYS.values()}

REGS = {'ax': UC_X86_REG_AX, 'bx': UC_X86_REG_BX, 'cx': UC_X86_REG_CX, 'dx': UC_X86_REG_DX,
        'si': UC_X86_REG_SI, 'di': UC_X86_REG_DI, 'bp': UC_X86_REG_BP, 'sp': UC_X86_REG_SP,
        'cs': UC_X86_REG_CS, 'ds': UC_X86_REG_DS, 'es': UC_X86_REG_ES, 'ss': UC_X86_REG_SS,
        'ip': UC_X86_REG_IP, 'flags': UC_X86_REG_EFLAGS}


class MachineError(RuntimeError):
    pass


def default_palette():
    """VGA BIOS default DAC contents for mode 13h (6-bit components): 16 EGA
    colours, 16 greys, nine 24-hue rings, 8 black entries."""
    ega = [(0, 0, 0), (0, 0, 42), (0, 42, 0), (0, 42, 42), (42, 0, 0), (42, 0, 42),
           (42, 21, 0), (42, 42, 42), (21, 21, 21), (21, 21, 63), (21, 63, 21),
           (21, 63, 63), (63, 21, 21), (63, 21, 63), (63, 63, 21), (63, 63, 63)]
    grey = [0, 5, 8, 11, 14, 17, 20, 24, 28, 32, 36, 40, 45, 50, 56, 63]
    pal = ega + [(g, g, g) for g in grey]
    for a, b, c, d, e in ((0, 16, 31, 47, 63), (31, 39, 47, 55, 63), (45, 49, 54, 58, 63),
                          (0, 7, 14, 21, 28), (14, 17, 21, 24, 28), (20, 22, 24, 26, 28),
                          (0, 4, 8, 12, 16), (8, 10, 12, 14, 16), (11, 12, 13, 15, 16)):
        up, down = (a, b, c, d), (e, d, c, b)
        ring = [(v, a, e) for v in up] + [(e, a, v) for v in down]
        ring += [(e, v, a) for v in up] + [(v, e, a) for v in down]
        ring += [(a, e, v) for v in up] + [(a, v, e) for v in down]
        pal += ring
    pal += [(0, 0, 0)] * (256 - len(pal))
    return bytearray(c for rgb in pal for c in rgb)


class GuestFiles:
    """Flat C:\\ directory: read-only originals plus copies written by the guest."""

    def __init__(self, files):
        self.files = {self.norm(k): bytearray(v) for k, v in files.items()}
        self.written = set()

    @staticmethod
    def norm(name):
        name = name.replace('/', '\\').split('\\')[-1]
        if len(name) >= 2 and name[1] == ':':
            name = name[2:]
        return name.upper()


class Machine:
    def __init__(self, exe, files, delay_entry, label='guest', load_seg=0x1000,
                 start_time=(12, 0, 0), step_budget=60_000_000):
        """exe: MZ bytes. files: {name: bytes} for C:\\. delay_entry: (segment
        relative to the load segment, offset) of Crt.Delay in this build."""
        self.label = label
        self.load_seg = load_seg
        self.psp = load_seg - 0x10
        self.fs = GuestFiles(files)
        self.step_budget = step_budget
        self.start_time = start_time
        uc = Uc(UC_ARCH_X86, UC_MODE_16)
        self.uc = uc
        uc.mem_map(0, 0x110000)
        self._load_mz(exe)
        self._init_bios()
        self.delay_linear = (load_seg + delay_entry[0]) * 16 + delay_entry[1]
        uc.hook_add(UC_HOOK_INTR, self._on_int)
        uc.hook_add(UC_HOOK_INSN, self._on_in, None, 1, 0, UC_X86_INS_IN)
        uc.hook_add(UC_HOOK_INSN, self._on_out, None, 1, 0, UC_X86_INS_OUT)
        uc.hook_add(UC_HOOK_CODE, self._on_delay, begin=self.delay_linear, end=self.delay_linear)
        uc.hook_add(UC_HOOK_MEM_READ, self._on_tick_read, begin=0x46C, end=0x46F)
        # virtual clock and devices
        self.vtime = 0.0
        self.ticks = 0                     # timer interrupts delivered
        self.busy_ticks = 0                # BIOS count advances for busy polling
        self.tick_reads = 0
        self.port60 = 0
        self.dac = default_palette()
        self.dac_write = 0
        self.dac_read = 0
        self.dac_phase = 0
        self.status_toggle = 0
        self.pit_latch = []
        self.pit2 = None
        self.port61 = 0
        self.speaker_log = []
        self.kbd_commands = []
        self.dos_pending = None
        self.inputs = []
        self.input_pos = 0
        self.polls = 0
        self.event = None
        self.exit_code = None
        self.console = bytearray()
        self.video_mode = 3
        self.cursor = (0, 0)
        self.text = bytearray(b' \x07' * 2000)
        self.dta = (self.psp, 0x80)
        self.handles = {}
        self.next_handle = 5
        self.blocks = {}                   # allocated DOS blocks: segment -> paragraphs
        self.prog_end = TOP_SEG
        self.int_counts = {}
        self.delays = 0
        self.find_state = None

    # -- loading -----------------------------------------------------------
    def _load_mz(self, data):
        if len(data) < 28 or data[:2] != b'MZ':
            raise MachineError('not an MZ executable')
        h = struct.unpack_from('<14H', data)
        header = h[4] * 16
        end = (h[2] - 1) * 512 + (h[1] or 512)
        image = bytearray(data[header:end])
        for i in range(h[3]):
            off, seg = struct.unpack_from('<HH', data, h[12] + 4 * i)
            at = seg * 16 + off
            word = struct.unpack_from('<H', image, at)[0]
            struct.pack_into('<H', image, at, (word + self.load_seg) & 0xFFFF)
        uc = self.uc
        uc.mem_write(self.load_seg * 16, bytes(image))
        self.image_size = len(image)
        psp = self.psp * 16
        # memory control block of the program's allocation, as DOS keeps it
        # just below the PSP (the Free Pascal start-up reads its size)
        self._write_mcb(TOP_SEG - self.psp)
        uc.mem_write(psp, b'\xcd\x20')
        uc.mem_write(psp + 2, struct.pack('<H', TOP_SEG))
        uc.mem_write(psp + 0x2C, struct.pack('<H', ENV_SEG))
        uc.mem_write(psp + 0x80, b'\x00\x0d')
        env = b'COMSPEC=C:\\COMMAND.COM\0PATH=C:\\\0\0' + struct.pack('<H', 1) + b'C:\\LEZAC.EXE\0'
        uc.mem_write(ENV_SEG * 16, env)
        self.entry = (self.load_seg + h[11], h[10])
        for reg, value in (('cs', self.load_seg + h[11]), ('ip', h[10]),
                           ('ss', self.load_seg + h[7]), ('sp', h[8]),
                           ('ds', self.psp), ('es', self.psp), ('flags', 0x0202)):
            self.reg(reg, value)

    def _write_mcb(self, paragraphs):
        self.uc.mem_write((self.psp - 1) * 16, b'Z' + struct.pack('<HH', self.psp, paragraphs) + bytes(11))

    def _init_bios(self):
        uc = self.uc
        stubs = bytearray()
        for n in range(256):
            stubs += bytes([0xCD, n, 0xCF, 0x90])          # int n ; iret
        uc.mem_write(BIOS_SEG * 16 + STUB_OFS, bytes(stubs))
        # hardware IRQ entries: timer -> BIOS tick + INT 1Ch; keyboard -> buffer
        self.irq8 = STUB_OFS + 0x400
        uc.mem_write(BIOS_SEG * 16 + self.irq8, bytes([0xCD, HLE_TICK, 0xCD, 0x1C, 0xCF]))
        self.irq9 = STUB_OFS + 0x410
        uc.mem_write(BIOS_SEG * 16 + self.irq9, bytes([0xCD, HLE_KEY, 0xCF]))
        uc.mem_write(BIOS_SEG * 16 + SENTINEL[1], b'\xf4')
        ivt = bytearray()
        for n in range(256):
            off = self.irq8 if n == 8 else self.irq9 if n == 9 else STUB_OFS + 4 * n
            ivt += struct.pack('<HH', off, BIOS_SEG)
        uc.mem_write(0, bytes(ivt))
        bda = 0x400
        uc.mem_write(bda + 0x10, struct.pack('<H', 0x0021))      # equipment: 80x25 colour, FPU no
        uc.mem_write(bda + 0x13, struct.pack('<H', 640))
        uc.mem_write(bda + 0x1A, struct.pack('<HH', 0x1E, 0x1E))  # keyboard buffer head/tail
        uc.mem_write(bda + 0x49, bytes([3]))
        uc.mem_write(bda + 0x4A, struct.pack('<H', 80))
        uc.mem_write(bda + 0x4C, struct.pack('<H', 4096))
        uc.mem_write(bda + 0x63, struct.pack('<H', 0x3D4))
        uc.mem_write(bda + 0x80, struct.pack('<HH', 0x1E, 0x3E))  # buffer start/end
        uc.mem_write(bda + 0x84, bytes([24]))
        uc.mem_write(bda + 0x85, struct.pack('<H', 16))

    # -- registers and memory ---------------------------------------------
    def reg(self, name, value=None):
        if value is None:
            return self.uc.reg_read(REGS[name])
        self.uc.reg_write(REGS[name], value & 0xFFFF if name != 'flags' else value)

    def read(self, linear, size):
        return bytes(self.uc.mem_read(linear, size))

    def write(self, linear, data):
        self.uc.mem_write(linear, bytes(data))

    def word(self, linear):
        return struct.unpack('<H', self.read(linear, 2))[0]

    def setcf(self, on):
        f = self.reg('flags')
        self.reg('flags', (f | 1) if on else (f & ~1))

    def setzf(self, on):
        f = self.reg('flags')
        self.reg('flags', (f | 0x40) if on else (f & ~0x40))

    def asciiz(self, seg, off):
        out = bytearray()
        base = seg * 16 + off
        while len(out) < 128:
            b = self.read(base + len(out), 1)[0]
            if b == 0:
                break
            out.append(b)
        return out.decode('latin-1')

    def push(self, value):
        sp = (self.reg('sp') - 2) & 0xFFFF
        self.reg('sp', sp)
        self.write(self.reg('ss') * 16 + sp, struct.pack('<H', value & 0xFFFF))

    def vector(self, n):
        off, seg = struct.unpack('<HH', self.read(n * 4, 4))
        return seg, off

    # -- input ---------------------------------------------------------------
    def schedule(self, events):
        """events: compiled trace events (t_ms, down/up/checkpoint/end...)."""
        self.inputs = [e for e in events if 'down' in e or 'up' in e]
        self.input_pos = 0

    def next_input_time(self):
        if self.input_pos < len(self.inputs):
            return self.inputs[self.input_pos]['t_ms']
        return None

    # -- hooks -----------------------------------------------------------------
    def _on_delay(self, uc, address, size, user):
        ss, sp = self.reg('ss'), self.reg('sp')
        ip, cs, ms = struct.unpack('<HHH', self.read(ss * 16 + sp, 6))
        self.reg('sp', sp + 6)
        self.reg('cs', cs)
        self.reg('ip', ip)
        self.event = ('delay', ms)
        uc.emu_stop()

    def _on_tick_read(self, uc, access, address, size, value, user):
        self.tick_reads += 1
        if self.tick_reads > 64:
            self.tick_reads = 0
            self.busy_ticks += 1
            self._write_bios_ticks()

    def _write_bios_ticks(self):
        self.write(0x46C, struct.pack('<I', (self.ticks + self.busy_ticks) & 0xFFFFFFFF))

    def _on_in(self, uc, port, size, user):
        if port == 0x60:
            return self.port60
        if port == 0x61:
            return self.port61
        if port == 0x3DA:
            self.status_toggle ^= 0x09
            return self.status_toggle
        if port == 0x3C9:
            value = self.dac[(self.dac_read * 3 + self.dac_phase) % 768]
            self.dac_phase += 1
            if self.dac_phase == 3:
                self.dac_phase = 0
                self.dac_read = (self.dac_read + 1) & 0xFF
            return value
        if port == 0x21:
            return 0
        if port in (0x40, 0x41, 0x42):
            return 0
        raise MachineError(f'{self.label}: IN from port {port:#x} at {self.where()}')

    def _on_out(self, uc, port, size, value, user):
        if size == 2:
            self._on_out(uc, port, 1, value & 0xFF, user)
            self._on_out(uc, port + 1, 1, value >> 8, user)
            return
        value &= 0xFF
        if port == 0x3C8:
            self.dac_write, self.dac_phase = value, 0
        elif port == 0x3C7:
            self.dac_read, self.dac_phase = value, 0
        elif port == 0x3C9:
            self.dac[(self.dac_write * 3 + self.dac_phase) % 768] = value & 0x3F
            self.dac_phase += 1
            if self.dac_phase == 3:
                self.dac_phase = 0
                self.dac_write = (self.dac_write + 1) & 0xFF
        elif port in (0x20, 0x21, 0xA0, 0xA1):
            pass
        elif port == 0x43:
            self.pit_latch = []
            self.speaker_log.append(('ctl', value))
        elif port == 0x42:
            self.pit_latch.append(value)
            if len(self.pit_latch) == 2:
                self.pit2 = self.pit_latch[0] | (self.pit_latch[1] << 8)
                self.pit_latch = []
                self.speaker_log.append(('div', self.pit2))
        elif port == 0x61:
            self.port61 = value
            self.speaker_log.append(('gate', value & 3))
        elif port in (0x3C4, 0x3C5, 0x3CE, 0x3CF, 0x3D4, 0x3D5):
            pass
        elif port in (0x60, 0x64):
            self.kbd_commands.append((port, value))   # e.g. typematic rate setting
        else:
            raise MachineError(f'{self.label}: OUT {value:#x} to port {port:#x} at {self.where()}')

    def where(self):
        return f'{self.reg("cs"):04X}:{self.reg("ip"):04X}'

    def _on_int(self, uc, n, user):
        cs, ip = self.reg('cs'), self.reg('ip')
        self.int_counts[n] = self.int_counts.get(n, 0) + 1
        if n == HLE_TICK:
            self.ticks_bios_advance()
            return
        if n == HLE_KEY:
            self._bios_key()
            return
        vec = self.vector(n)
        if cs != BIOS_SEG and vec[0] != BIOS_SEG:
            # a guest-installed handler: vector through the IVT as the CPU does
            self.push(self.reg('flags'))
            self.push(cs)
            self.push(ip)
            self.reg('flags', self.reg('flags') & ~0x300)
            self.reg('cs', vec[0])
            self.reg('ip', vec[1])
            return
        handler = getattr(self, f'_int{n:02x}', None)
        if handler is None:
            raise MachineError(f'{self.label}: unsupported INT {n:02X}h AX={self.reg("ax"):04X} at {cs:04X}:{ip:04X}')
        handler()

    def ticks_bios_advance(self):
        self._write_bios_ticks()

    def _bios_key(self):
        make = self.port60
        if make & 0x80:
            return
        ascii_ = MAKE_TO_ASCII.get(make, 0)
        head, tail = struct.unpack('<HH', self.read(0x41A, 4))
        nxt = tail + 2
        if nxt >= 0x3E:
            nxt = 0x1E
        if nxt == head:
            return                          # buffer full: the BIOS beeps and drops it
        self.write(0x400 + tail, bytes([ascii_, make]))
        self.write(0x41C, struct.pack('<H', nxt))

    # -- BIOS services -------------------------------------------------------
    def _int10(self):
        ax = self.reg('ax')
        ah, al = ax >> 8, ax & 0xFF
        if ah == 0x00:
            mode = al & 0x7F
            self.video_mode = mode
            self.write(0x449, bytes([mode]))
            if mode == 0x13:
                if not al & 0x80:
                    self.write(VIDEO, bytes(65536))
                self.write(0x44A, struct.pack('<H', 40))
                self.dac = default_palette()
            else:
                self.write(0x44A, struct.pack('<H', 80))
                self.text = bytearray(b' \x07' * 2000)
                self.cursor = (0, 0)
        elif ah == 0x0F:
            cols = 40 if self.video_mode == 0x13 else 80
            self.reg('ax', (cols << 8) | self.video_mode)
            self.reg('bx', self.reg('bx') & 0x00FF)
        elif ah == 0x02:
            dx = self.reg('dx')
            self.cursor = (dx >> 8, dx & 0xFF)
        elif ah == 0x03:
            self.reg('dx', (self.cursor[0] << 8) | self.cursor[1])
            self.reg('cx', 0x0607)
        elif ah == 0x01:
            pass
        elif ah == 0x08:
            row, col = self.cursor
            at = (row * 80 + col) * 2
            self.reg('ax', self.text[at] | (self.text[at + 1] << 8))
        elif ah in (0x09, 0x0A):
            row, col = self.cursor
            count = self.reg('cx')
            attr = self.reg('bx') & 0xFF
            for i in range(count):
                at = (row * 80 + col + i) * 2
                if at < 4000:
                    self.text[at] = al
                    if ah == 0x09:
                        self.text[at + 1] = attr
            self.console.append(al)
        elif ah == 0x0E:
            self._teletype(al)
        elif ah in (0x06, 0x07):
            lines = al
            ch, cl = self.reg('cx') >> 8, self.reg('cx') & 0xFF
            dh, dl = self.reg('dx') >> 8, self.reg('dx') & 0xFF
            attr = self.reg('bx') >> 8
            rows = list(range(ch, dh + 1))
            if lines == 0 or lines > len(rows):
                for r in rows:
                    for c in range(cl, dl + 1):
                        self.text[(r * 80 + c) * 2:(r * 80 + c) * 2 + 2] = bytes([32, attr])
            else:
                order = rows if ah == 0x06 else rows[::-1]
                step = lines if ah == 0x06 else -lines
                for r in order:
                    src = r + step
                    for c in range(cl, dl + 1):
                        at = (r * 80 + c) * 2
                        if ch <= src <= dh:
                            sa = (src * 80 + c) * 2
                            self.text[at:at + 2] = self.text[sa:sa + 2]
                        else:
                            self.text[at:at + 2] = bytes([32, attr])
        elif ah == 0x10:
            if al == 0x12:
                es, dx = self.reg('es'), self.reg('dx')
                first, count = self.reg('bx'), self.reg('cx')
                data = self.read(es * 16 + dx, count * 3)
                for i in range(count * 3):
                    self.dac[(first * 3 + i) % 768] = data[i] & 0x3F
            elif al == 0x10:
                index = self.reg('bx')
                dh, cx = self.reg('dx') >> 8, self.reg('cx')
                self.dac[index * 3:index * 3 + 3] = bytes([dh & 63, (cx >> 8) & 63, cx & 63])
        elif ah in (0x11, 0x12):
            if ah == 0x12 and (self.reg('bx') & 0xFF) == 0x10:
                self.reg('bx', 0x0003)
                self.reg('cx', 0x0009)
        elif ah == 0x1A:
            self.reg('ax', 0x001A)
            self.reg('bx', 0x0008)
        else:
            raise MachineError(f'{self.label}: INT 10h AX={ax:04X} unsupported at {self.where()}')

    def _teletype(self, ch):
        self.console.append(ch)
        row, col = self.cursor
        if ch == 0x0D:
            col = 0
        elif ch == 0x0A:
            row += 1
        elif ch == 0x08:
            col = max(0, col - 1)
        elif ch == 0x07:
            pass
        else:
            at = (row * 80 + col) * 2
            if at < 4000:
                self.text[at] = ch
            col += 1
            if col >= 80:
                col, row = 0, row + 1
        if row >= 25:
            self.text = self.text[160:] + bytearray(b' \x07' * 80)
            row = 24
        self.cursor = (row, col)

    def _int11(self):
        self.reg('ax', self.word(0x410))

    def _int12(self):
        self.reg('ax', 640)

    def _int16(self):
        ax = self.reg('ax')
        ah = ax >> 8
        head, tail = struct.unpack('<HH', self.read(0x41A, 4))
        if ah in (0x00, 0x10):
            if head == tail:
                # Blocking read: re-execute this INT after the next key arrives.
                self.reg('ip', self.reg('ip') - 2)
                self.event = ('wait_key',)
                self.uc.emu_stop()
                return
            word = self.word(0x400 + head)
            nxt = head + 2 if head + 2 < 0x3E else 0x1E
            self.write(0x41A, struct.pack('<H', nxt))
            self.reg('ax', word)
            self.polls = 0
        elif ah in (0x01, 0x11):
            if head == tail:
                self.setzf(True)
                self.polls += 1
                if self.polls >= POLL_LIMIT:
                    self.polls = 0
                    self.event = ('idle',)
                    self.uc.emu_stop()
            else:
                self.reg('ax', self.word(0x400 + head))
                self.setzf(False)
        elif ah in (0x02, 0x12):
            self.reg('ax', (ax & 0xFF00) | 0)
        elif ah == 0x03:
            pass
        else:
            raise MachineError(f'{self.label}: INT 16h AX={ax:04X} unsupported')

    def _int1a(self):
        ah = self.reg('ax') >> 8
        if ah == 0x00:
            t = self.ticks + self.busy_ticks
            self.reg('cx', t >> 16)
            self.reg('dx', t & 0xFFFF)
            self.reg('ax', 0)
        else:
            raise MachineError(f'{self.label}: INT 1Ah AH={ah:02X} unsupported')

    def _int1c(self):
        pass

    def _int1b(self):
        pass

    def _int23(self):
        pass

    # -- DOS ---------------------------------------------------------------------
    def dos_time(self):
        """DOS clock derived from the delivered timer ticks only (the busy
        polling advances of the BIOS count are not wall time)."""
        h, m, s = self.start_time
        hundredths = int(self.ticks * TICK_MS / 10)
        total = ((h * 60 + m) * 60 + s) * 100 + hundredths
        total %= 24 * 360000
        hh, rest = divmod(total, 360000)
        mm, rest = divmod(rest, 6000)
        ss, cc = divmod(rest, 100)
        return hh, mm, ss, cc

    def _int21(self):
        ax = self.reg('ax')
        ah, al = ax >> 8, ax & 0xFF
        bx, cx, dx = self.reg('bx'), self.reg('cx'), self.reg('dx')
        ds, es = self.reg('ds'), self.reg('es')
        self.setcf(False)
        if ah == 0x71:
            self.reg('ax', 0x7100)              # no long-file-name services (DOS 5)
            self.setcf(True)
        elif ah == 0x30:
            self.reg('ax', 0x0005)
            self.reg('bx', 0)
            self.reg('cx', 0)
        elif ah == 0x35:
            seg, off = self.vector(al)
            self.reg('es', seg)
            self.reg('bx', off)
        elif ah == 0x25:
            self.write(al * 4, struct.pack('<HH', dx, ds))
        elif ah == 0x2C:
            hh, mm, ss, cc = self.dos_time()
            self.reg('cx', (hh << 8) | mm)
            self.reg('dx', (ss << 8) | cc)
        elif ah == 0x2A:
            self.reg('cx', 1996)
            self.reg('dx', (4 << 8) | 23)
            self.reg('ax', (ax & 0xFF00) | 2)
        elif ah == 0x33:
            if al == 0:
                self.reg('dx', dx & 0xFF00)
            elif al == 5:
                self.reg('dx', (dx & 0xFF00) | 3)
        elif ah == 0x19:
            self.reg('ax', (ax & 0xFF00) | 2)
        elif ah == 0x0E:
            self.reg('ax', (ax & 0xFF00) | 3)
        elif ah == 0x47:
            self.write(ds * 16 + self.reg('si'), b'\0')
        elif ah == 0x62 or ah == 0x51:
            self.reg('bx', self.psp)
        elif ah == 0x1A:
            self.dta = (ds, dx)
        elif ah == 0x2F:
            self.reg('es', self.dta[0])
            self.reg('bx', self.dta[1])
        elif ah == 0x44:
            if al == 0x00:
                if bx in (0, 1, 2):
                    self.reg('dx', 0x80 | (1 if bx == 0 else 2 if bx == 1 else 0) | 0x0080)
                elif bx in self.handles:
                    self.reg('dx', 0x0002)
                else:
                    self.dos_error(6)
                    return
            elif al == 0x01:
                pass
            else:
                self.dos_error(1)
        elif ah == 0x3D:
            name = self.fs.norm(self.asciiz(ds, dx))
            if name not in self.fs.files:
                self.dos_error(2)
                return
            self.open_handle(name, al & 3)
        elif ah == 0x3C:
            name = self.fs.norm(self.asciiz(ds, dx))
            self.fs.files[name] = bytearray()
            self.fs.written.add(name)
            self.open_handle(name, 2)
        elif ah == 0x3E:
            if bx in self.handles:
                del self.handles[bx]
            elif bx > 4:
                self.dos_error(6)
        elif ah == 0x3F:
            if bx in (0, 3, 4):
                self.reg('ax', 0)
                return
            h = self.handles.get(bx)
            if h is None:
                self.dos_error(6)
                return
            data = self.fs.files[h['name']]
            chunk = data[h['pos']:h['pos'] + cx]
            self.write(ds * 16 + dx, chunk)
            h['pos'] += len(chunk)
            self.reg('ax', len(chunk))
        elif ah == 0x40:
            payload = self.read(ds * 16 + dx, cx)
            if bx in (1, 2):
                for ch in payload:
                    self._teletype(ch)
                self.reg('ax', cx)
                return
            h = self.handles.get(bx)
            if h is None:
                self.dos_error(6)
                return
            data = self.fs.files[h['name']]
            if cx == 0:
                del data[h['pos']:]
            else:
                if len(data) < h['pos']:
                    data.extend(bytes(h['pos'] - len(data)))
                data[h['pos']:h['pos'] + cx] = payload
                h['pos'] += cx
            self.fs.written.add(h['name'])
            self.reg('ax', cx)
        elif ah == 0x42:
            h = self.handles.get(bx)
            if h is None:
                self.dos_error(6)
                return
            offset = (cx << 16) | dx
            if offset & 0x80000000:
                offset -= 1 << 32
            base = 0 if al == 0 else h['pos'] if al == 1 else len(self.fs.files[h['name']])
            pos = base + offset
            if pos < 0:
                self.dos_error(0x19)
                return
            h['pos'] = pos
            self.reg('dx', pos >> 16)
            self.reg('ax', pos & 0xFFFF)
        elif ah == 0x41:
            name = self.fs.norm(self.asciiz(ds, dx))
            if name in self.fs.files:
                del self.fs.files[name]
            else:
                self.dos_error(2)
        elif ah == 0x43:
            name = self.fs.norm(self.asciiz(ds, dx))
            if name not in self.fs.files:
                self.dos_error(2)
                return
            self.reg('cx', 0x20)
        elif ah == 0x4E:
            pattern = self.fs.norm(self.asciiz(ds, dx))
            names = sorted(n for n in self.fs.files if fnmatch.fnmatch(n, pattern.replace('*.*', '*')))
            self.find_state = names
            self.find_next()
        elif ah == 0x4F:
            self.find_next()
        elif ah == 0x48:
            self.dos_alloc(bx)
        elif ah == 0x49:
            if es in self.blocks:
                del self.blocks[es]
            else:
                self.dos_error(9)
        elif ah == 0x4A:
            self.dos_resize(es, bx)
        elif ah == 0x4C:
            self.exit_code = al
            self.event = ('exit', al)
            self.uc.emu_stop()
        elif ah == 0x0B:
            head, tail = struct.unpack('<HH', self.read(0x41A, 4))
            ready = head != tail or self.dos_pending is not None
            self.reg('ax', (ax & 0xFF00) | (0xFF if ready else 0))
        elif ah == 0x0C:
            self.write(0x41A, struct.pack('<H', self.word(0x41C)))   # flush type-ahead
            self.dos_pending = None
            if al in (0x01, 0x06, 0x07, 0x08):
                self.reg('ax', (al << 8) | al)
                self._dos_char_io(al, dx, rewind=0)
        elif ah in (0x01, 0x06, 0x07, 0x08):
            self._dos_char_io(ah, dx, rewind=2)
        elif ah == 0x09:
            seg = ds * 16 + dx
            for i in range(4096):
                ch = self.read(seg + i, 1)[0]
                if ch == ord('$'):
                    break
                self._teletype(ch)
        elif ah == 0x02:
            self._teletype(dx & 0xFF)
        else:
            raise MachineError(f'{self.label}: INT 21h AX={ax:04X} BX={bx:04X} unsupported at {self.where()}')

    def _dos_char_io(self, function, dx, rewind):
        if function == 0x06 and (dx & 0xFF) != 0xFF:
            self._teletype(dx & 0xFF)
            return
        if self.dos_pending is not None:
            char, self.dos_pending = self.dos_pending, None
        else:
            head, tail = struct.unpack('<HH', self.read(0x41A, 4))
            if head == tail:
                if function == 0x06:
                    self.reg('ax', self.reg('ax') & 0xFF00)
                    self.setzf(True)
                    return
                if rewind:
                    self.reg('ip', self.reg('ip') - rewind)
                    self.event = ('wait_key',)
                    self.uc.emu_stop()
                    return
                raise MachineError(f'{self.label}: blocking console read after flush at {self.where()}')
            word = self.word(0x400 + head)
            self.write(0x41A, struct.pack('<H', head + 2 if head + 2 < 0x3E else 0x1E))
            char = word & 0xFF
            if char == 0:
                self.dos_pending = word >> 8
        self.reg('ax', (self.reg('ax') & 0xFF00) | char)
        if function == 0x06:
            self.setzf(False)
        elif function == 0x01:
            self._teletype(char)

    def _int20(self):
        self.exit_code = 0
        self.event = ('exit', 0)
        self.uc.emu_stop()

    def dos_error(self, code):
        self.reg('ax', code)
        self.setcf(True)

    def open_handle(self, name, mode):
        handle = self.next_handle
        self.next_handle += 1
        self.handles[handle] = {'name': name, 'pos': 0, 'mode': mode}
        self.reg('ax', handle)

    def find_next(self):
        if not self.find_state:
            self.dos_error(0x12)
            return
        name = self.find_state.pop(0)
        dta = self.dta[0] * 16 + self.dta[1]
        entry = bytearray(43)
        entry[0x15] = 0x20
        struct.pack_into('<HHI', entry, 0x16, 0x6000, 0x2097, len(self.fs.files[name]))
        entry[0x1E:0x1E + len(name)] = name.encode('latin-1')
        self.write(dta, entry)
        self.reg('ax', 0)

    def _free_ranges(self):
        used = sorted([(self.psp, self.prog_end)] + [(s, s + n) for s, n in self.blocks.items()])
        ranges, at = [], None
        for start, end in used:
            if at is not None and start > at:
                ranges.append((at, start))
            at = max(at or 0, end)
        if at < TOP_SEG:
            ranges.append((at, TOP_SEG))
        return ranges

    def dos_alloc(self, paragraphs):
        best = 0
        for start, end in self._free_ranges():
            if end - start - 1 >= paragraphs:
                self.blocks[start + 1] = paragraphs
                self.reg('ax', start + 1)
                return
            best = max(best, end - start - 1)
        self.reg('bx', max(best, 0))
        self.dos_error(8)

    def dos_resize(self, segment, paragraphs):
        if segment == self.psp:
            limit = TOP_SEG
            for start in self.blocks:
                if start > self.psp:
                    limit = min(limit, start - 1)
            if self.psp + paragraphs > limit:
                self.reg('bx', limit - self.psp)
                self.dos_error(8)
                return
            self.prog_end = self.psp + paragraphs
            self._write_mcb(paragraphs)
        elif segment in self.blocks:
            self.blocks[segment] = paragraphs
        else:
            self.dos_error(9)

    # -- execution ----------------------------------------------------------------
    def call_interrupt(self, n, limit=2_000_000):
        """Run hardware interrupt n now (between program instructions)."""
        saved = {r: self.reg(r) for r in ('cs', 'ip')}
        self.push(self.reg('flags'))
        self.push(SENTINEL[0])
        self.push(SENTINEL[1])
        seg, off = self.vector(n)
        self.reg('flags', self.reg('flags') & ~0x300)
        self.reg('cs', seg)
        self.reg('ip', off)
        self.event = None
        self.uc.emu_start(seg * 16 + off, SENTINEL[0] * 16 + SENTINEL[1], count=limit)
        if (self.reg('cs'), self.reg('ip')) != SENTINEL:
            raise MachineError(f'{self.label}: interrupt {n:02X}h did not return ({self.event}) at {self.where()}')
        for r, v in saved.items():
            self.reg(r, v)

    def advance(self, until_ms):
        """Advance virtual time, delivering timer ticks and due key events."""
        while True:
            next_tick = (self.ticks + 1) * TICK_MS
            nxt_in = self.next_input_time()
            candidates = [t for t in (next_tick, nxt_in) if t is not None and t <= until_ms]
            if not candidates:
                break
            t = min(candidates)
            self.vtime = max(self.vtime, t)
            if nxt_in is not None and nxt_in <= next_tick and nxt_in <= until_ms:
                ev = self.inputs[self.input_pos]
                self.input_pos += 1
                key = ev.get('down') or ev.get('up')
                make = KEYS[key][0]
                self.port60 = make if 'down' in ev else make | 0x80
                self.call_interrupt(9)
            else:
                self.ticks += 1
                self.call_interrupt(8)
        self.vtime = max(self.vtime, until_ms)

    def step(self):
        """Run the program to its next boundary; returns the event."""
        while True:
            self.event = None
            cs, ip = self.reg('cs'), self.reg('ip')
            try:
                self.uc.emu_start(cs * 16 + ip, 0x10FFF0, count=self.step_budget)
            except UcError as error:
                raise MachineError(f'{self.label}: {error} at {self.where()}') from error
            event = self.event
            if event is None:
                raise MachineError(f'{self.label}: no Delay/input/exit within {self.step_budget} instructions at {self.where()}')
            if event[0] == 'delay':
                self.delays += 1
                self.polls = 0
                self.advance(self.vtime + event[1])
                return event
            if event[0] in ('wait_key', 'idle'):
                target = self.next_input_time()
                if target is None:
                    return ('stalled',)
                self.advance(max(target, self.vtime))
                return event
            if event[0] == 'exit':
                return event

    def framebuffer(self):
        return self.read(VIDEO, 64000)

    def palette(self):
        return bytes(self.dac)
