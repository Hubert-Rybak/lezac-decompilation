#!/usr/bin/env python3
"""Lockstep comparison of the ORIGINAL LEZAC.EXE and the source-built rebuild.

Both executables run in their own deterministic machine (tools/dosmachine.py)
fed with the same input trace (the lezac-input-trace/1 format of
tools/run_original.py; times are virtual milliseconds). After every program
boundary (a Crt.Delay call, an input wait or the exit) the two machines are
compared:

  * the boundary itself (kind and Delay argument),
  * the 320x200 VGA framebuffer and the 256-colour DAC palette,
  * the PC-speaker port writes (PIT channel 2 divisors, speaker gate),
  * the named global variables of the program and of the graphics unit
    (original: DS offsets from the comments of pascal/LZDATA.INC and
    pascal/LZGRAPH.PAS; rebuild: symbol addresses from the build's map and
    object files), excluding pointers, file records and values that are
    addresses by nature (see ADDRESS_VALUED); heap blocks reached through
    pointers are compared by content.

The first divergence is reported with its boundary number and details. No
image of the game is written unless --shots is given (keep such files local).
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import sys
import time

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from dosmachine import Machine, MachineError  # noqa: E402
from fpcsyms import symbol_table  # noqa: E402
from run_original import compile_trace  # noqa: E402

ORIGINAL_DIR = ROOT / 'original' / 'game'
ORIGINAL_SHA256 = '7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec'
ORIGINAL_DELAY = (0x084A, 0x029C)        # Crt.Delay = 184A:029C at load segment 1000h
ORIGINAL_DS = 0x0AA2                     # data segment relative to the load segment
LOAD_SEG = 0x1000

TYPE_SIZE = {'Byte': 1, 'Char': 1, 'Shortint': 1, 'Boolean': 1, 'Word': 2, 'Integer': 2,
             'LongInt': 4, 'Longint': 4, 'Pointer': 4, 'TReal': 6, 'TRGB3': 3, 'TScore13': 13,
             'TActor38': 38, 'TDebris11': 11, 'TGroup15': 15, 'TSpawn30': 30, 'TMapPoint7': 7,
             'TTileReplacement14': 14, 'TScoreDisplay45': 45, 'TMotion16': 16,
             'TRenderSprite8': 8, 'TSpriteDef4': 4, 'TTileStore': 17004}
SKIP_TYPES = {'Pointer', 'file'}
# Words that hold an address (offset or segment) of the running program: they
# differ legitimately between two builds and are not compared directly.
ADDRESS_VALUED = {'TileBase', 'RenderTableOfs', 'MapSeg', 'BackSeg', 'BackOfs', 'SpriteSeg',
                  'MapBSeg', 'ScoreGap', 'DigitBase', 'Reserved1B68', 'GroupBase', 'DebrisBase'}
# Word fields that hold an offset into a block of the program: compared
# relative to that block. (array, element size, field offsets, base)
RELATIVE = {
    'SpriteDefs': (4, (2,), 'SpriteData'),
    'RenderEntries': (8, (6,), 'SpriteData'),
    'ScoreDisp1': (45, tuple(range(4, 0x2C, 2)), 'TileBase'),
    'ScoreDisp2': (45, tuple(range(4, 0x2C, 2)), 'TileBase'),
}
# Values copied from run-time library state that Free Pascal initialises
# differently (Crt.CheckBreak starts False in FPC, True in Turbo Pascal).
RUNTIME_DEPENDENT = {'SavedBreak'}
# Shared scratch variables: compared, but a difference alone is reported as a
# note, because they may transiently hold addresses.
SCRATCH = {'TmpA', 'TmpB', 'GTemp', 'RowTemp'}
DECL = re.compile(r'^\s*(\w+)\s*:\s*(.+?)\s*(?:=.*)?;\s*\{\s*DS:([0-9A-Fa-f]{4})')
CONST_DS = {'ScoreTable': 0x0000, 'BonusSprites': 0x001A, 'KindBomb': 0x0032, 'PickupScore': 0x0036,
            'PickupSprite': 0x0044, 'PickupChance': 0x0052, 'Anims': 0x005A, 'Texts': 0x008A,
            'LevelOffsets': 0x1A8A, 'FlashDark': 0x1AAE, 'FlashWhite': 0x1AB2, 'FlashRed': 0x1AB6,
            'FlashBlack': 0x1ABA, 'PlayerDigits': 0x1ABE}
CONST_SIZE = {'ScoreTable': 26, 'BonusSprites': 24, 'KindBomb': 4, 'PickupScore': 14,
              'PickupSprite': 14, 'PickupChance': 8, 'Anims': 48, 'Texts': 26 * 256,
              'LevelOffsets': 36, 'FlashDark': 3, 'FlashWhite': 3, 'FlashRed': 3, 'FlashBlack': 3,
              'PlayerDigits': 3}


def type_size(text):
    text = text.strip()
    m = re.match(r'array\[(-?\d+)\.\.(\d+)(?:,\s*(-?\d+)\.\.(\d+))?\]\s+of\s+(\w+)', text)
    if m:
        n = int(m.group(2)) - int(m.group(1)) + 1
        if m.group(3):
            n *= int(m.group(4)) - int(m.group(3)) + 1
        return n * TYPE_SIZE[m.group(5)], m.group(5)
    if text.startswith('string'):
        m = re.match(r'string\[(\d+)\]', text)
        return (int(m.group(1)) + 1 if m else 256), 'string'
    base = text.split()[0]
    return TYPE_SIZE.get(base, None), base


def variables():
    """[(name, owner, ds_offset, size, element type)] for compared globals."""
    out = []
    for path, owner in ((ROOT / 'pascal/LZDATA.INC', 'program'), (ROOT / 'pascal/LZGRAPH.PAS', 'LZGRAPH')):
        in_var = False
        for line in path.read_text().splitlines():
            if line.strip() == 'var':
                in_var = True
                continue
            if owner == 'LZGRAPH' and line.startswith('implementation'):
                break
            m = DECL.match(line)
            if not m or not in_var or ' absolute ' in line:
                continue
            name, typ, ds = m.group(1), m.group(2), int(m.group(3), 16)
            size, elem = type_size(typ)
            if size is None:
                size = 128 if typ.strip() == 'file' else None
            out.append((name, owner, ds, size, elem))
    for name, ds in CONST_DS.items():
        out.append((name, 'const', ds, CONST_SIZE[name], 'const'))
    return out


def fpc_symbol(name, owner):
    if owner == 'program':
        return f'U_$P$LEZAC_$$_{name.upper()}'
    if owner == 'const':
        # the text table is stored as zero-padded character arrays (see LZDATA.INC)
        return 'TC_$P$LEZAC_$$_TEXTSTORE' if name == 'Texts' else f'TC_$P$LEZAC_$$_{name.upper()}'
    return f'U_${owner}_$$_{name.upper()}'


class Build:
    """One executable plus how to find its variables."""

    def __init__(self, label, exe, delay, addresses, data_seg=None):
        self.label, self.exe, self.delay = label, exe, delay
        self.addresses = addresses          # name -> linear address (at LOAD_SEG)
        self.data_seg = data_seg


def original_build():
    exe = (ORIGINAL_DIR / 'LEZAC.EXE').read_bytes()
    if hashlib.sha256(exe).hexdigest() != ORIGINAL_SHA256:
        raise SystemExit('original/game/LEZAC.EXE is not the authenticated original')
    base = (LOAD_SEG + ORIGINAL_DS) * 16
    addresses = {name: base + ds for name, _owner, ds, _size, _elem in variables()}
    return Build('original', exe, ORIGINAL_DELAY, addresses, LOAD_SEG + ORIGINAL_DS)


def rebuilt_build(directory):
    directory = Path(directory)
    exe = (directory / 'LEZAC.EXE').read_bytes()
    table = symbol_table(directory / 'LEZAC.MAP', sorted((directory / 'obj').glob('*.o')))
    delay = table.get('CRT_$$_DELAY$WORD')
    if delay is None:
        raise SystemExit('Crt.Delay not found in the rebuild map')
    addresses, missing = {}, []
    for name, owner, _ds, _size, _elem in variables():
        sym = table.get(fpc_symbol(name, owner))
        if sym is None:
            missing.append(name)
            continue
        addresses[name] = (LOAD_SEG + sym[0]) * 16 + sym[1]
    if missing:
        print(f'note: rebuild symbols not found: {missing}', file=sys.stderr)
    return Build('rebuild', exe, delay, addresses)


def game_files():
    return {p.name: p.read_bytes() for p in ORIGINAL_DIR.iterdir() if p.is_file()}


def make_machine(build, events):
    machine = Machine(build.exe, game_files(), build.delay, label=build.label, load_seg=LOAD_SEG)
    machine.schedule(events)
    return machine


def far_pointer(machine, address):
    off, seg = struct.unpack('<HH', machine.read(address, 4))
    return seg * 16 + off


def heap_blocks(machine, build):
    """Contents reached through the program's pointers (size known from state)."""
    out = {}
    a = build.addresses
    def word(name):
        return struct.unpack('<H', machine.read(a[name], 2))[0]
    if 'LevelSize' in a:
        size = word('LevelSize')
        for name, scale in (('MapA', 1), ('MapB', 2)):
            if name in a:
                ptr = far_pointer(machine, a[name])
                if ptr and size:
                    out[name + '^'] = machine.read(ptr, min(size * scale, 0x10000))
    for name, size in (('Background', 60000), ('Sounds', 792)):
        if name in a:
            ptr = far_pointer(machine, a[name])
            if ptr:
                out[name + '^'] = machine.read(ptr, size)
    return out


def base_offset(machine, build, base):
    if base == 'TileBase':
        return struct.unpack('<H', machine.read(build.addresses['TileBase'], 2))[0]
    return struct.unpack('<H', machine.read(build.addresses[base], 2))[0]   # offset word of a pointer


def normalized(machine, build, name, size):
    data = bytearray(machine.read(build.addresses[name], size))
    rule = RELATIVE.get(name)
    if rule:
        element, fields, base = rule
        delta = base_offset(machine, build, base)
        for start in range(0, size - element + 1, element):
            for field in fields:
                at = start + field
                value = struct.unpack_from('<H', data, at)[0]
                if value:                       # 0 = never set in both builds
                    struct.pack_into('<H', data, at, (value - delta) & 0xFFFF)
    return bytes(data)


def compare_state(m1, b1, m2, b2):
    """List of (severity, description) differences between two machines."""
    diffs = []
    if m1.framebuffer() != m2.framebuffer():
        fb1, fb2 = m1.framebuffer(), m2.framebuffer()
        first = next(i for i in range(64000) if fb1[i] != fb2[i])
        count = sum(1 for i in range(64000) if fb1[i] != fb2[i])
        diffs.append(('error', f'framebuffer: {count} pixels differ, first at x={first % 320} y={first // 320} '
                               f'({fb1[first]} vs {fb2[first]})'))
    if m1.palette() != m2.palette():
        p1, p2 = m1.palette(), m2.palette()
        first = next(i for i in range(768) if p1[i] != p2[i])
        diffs.append(('error', f'palette: first difference at colour {first // 3} component {first % 3} '
                               f'({p1[first]} vs {p2[first]})'))
    if m1.speaker_log != m2.speaker_log:
        diffs.append(('error', f'speaker: {m1.speaker_log[-6:]} vs {m2.speaker_log[-6:]}'))
    for name, owner, _ds, size, elem in variables():
        if size is None or elem in SKIP_TYPES or name in ADDRESS_VALUED:
            continue
        if name not in b1.addresses or name not in b2.addresses:
            continue
        v1 = normalized(m1, b1, name, size)
        v2 = normalized(m2, b2, name, size)
        if v1 != v2:
            first = next(i for i in range(size) if v1[i] != v2[i])
            severity = 'note' if name in SCRATCH or name in RUNTIME_DEPENDENT else 'error'
            diffs.append((severity, f'{name} (+{first}): {v1[first:first + 8].hex()} vs {v2[first:first + 8].hex()}'))
    h1, h2 = heap_blocks(m1, b1), heap_blocks(m2, b2)
    for name in sorted(set(h1) | set(h2)):
        if h1.get(name) != h2.get(name):
            a, b = h1.get(name, b''), h2.get(name, b'')
            first = next((i for i in range(min(len(a), len(b))) if a[i] != b[i]), min(len(a), len(b)))
            diffs.append(('error', f'{name} (+{first}): {a[first:first + 8].hex()} vs {b[first:first + 8].hex()}'))
    return diffs


def save_shot(machine, path):
    from PIL import Image
    img = Image.frombytes('P', (320, 200), machine.framebuffer())
    img.putpalette([min(255, c * 4 + c // 16) for c in machine.palette()])
    img.convert('RGB').resize((640, 400), Image.NEAREST).save(path)


MILESTONES = (('Level', 0, 1), ('Players', 0, 1), ('Lives', 0, 1), ('Lives', 1, 1),
              ('PlayerState', 0, 1), ('PlayerState', 1, 1), ('BonusDone', 0, 1),
              ('DestructionDone', 0, 1), ('BonusCollected', 0, 2), ('Percent', 0, 1))


def milestone_values(machine, build):
    out = {}
    for name, index, size in MILESTONES:
        raw = machine.read(build.addresses[name] + index * size, size)
        out[f'{name}[{index + 1}]' if name in ('Lives', 'PlayerState') else name] = \
            int.from_bytes(raw, 'little')
    return out


def run_lockstep(build_dir, trace, max_steps, keep_going=False, shots=None, quiet=False):
    """build_dir=None compares the original with a second copy of itself
    (a determinism check of the machine model)."""
    events = compile_trace(trace)
    original = original_build()
    rebuild = rebuilt_build(build_dir) if build_dir is not None else original_build()
    m1, m2 = make_machine(original, events), make_machine(rebuild, events)
    report = {'format': 'lezac-lockstep/1', 'steps': 0, 'divergence': None, 'notes': [],
              'original_sha256': ORIGINAL_SHA256,
              'rebuild_sha256': hashlib.sha256(rebuild.exe).hexdigest(),
              'frame_digest': hashlib.sha256()}
    started = time.time()
    report['milestones'] = []
    last_values = None
    for step in range(1, max_steps + 1):
        try:
            e1 = m1.step()
        except MachineError as error:
            report['divergence'] = {'step': step, 'what': f'original failed: {error}'}
            break
        try:
            e2 = m2.step()
        except MachineError as error:
            report['divergence'] = {'step': step, 'what': f'rebuild failed: {error}', 'original_event': list(e1)}
            break
        report['steps'] = step
        diffs = []
        if e1 != e2:
            diffs.append(('error', f'boundary: original {e1} vs rebuild {e2}'))
        diffs += compare_state(m1, original, m2, rebuild)
        errors = [d for s, d in diffs if s == 'error']
        notes = [d for s, d in diffs if s == 'note']
        if notes and len(report['notes']) < 50:
            report['notes'].append({'step': step, 'notes': notes})
        report['frame_digest'].update(m1.framebuffer() + m1.palette())
        values = milestone_values(m1, original)
        if last_values is not None:
            changed = {k: [last_values[k], v] for k, v in values.items() if v != last_values[k]}
            if changed and len(report['milestones']) < 400:
                report['milestones'].append({'step': step, 'vtime_ms': round(m1.vtime, 1), 'changed': changed})
        last_values = values
        if shots and step in shots:
            save_shot(m1, Path(shots[step]).with_suffix('.original.png'))
            save_shot(m2, Path(shots[step]).with_suffix('.rebuild.png'))
        if errors:
            report.setdefault('divergences', []).append({'step': step, 'event': list(e1), 'vtime_ms': round(m1.vtime, 1),
                                                         'differences': errors[:20]})
            if report['divergence'] is None:
                report['divergence'] = report['divergences'][0]
            if not keep_going:
                break
        if not quiet and step % 500 == 0:
            print(f'step {step} t={m1.vtime / 1000:.1f}s {e1}', file=sys.stderr)
        if e1[0] in ('exit', 'stalled'):
            break
    report['final_event'] = list(e1) if report['steps'] else None
    report['final_state'] = last_values
    report['vtime_ms'] = round(m1.vtime, 1)
    report['frame_digest'] = report['frame_digest'].hexdigest()
    report['seconds'] = round(time.time() - started, 1)
    report['console_original'] = bytes(m1.console).decode('cp437', 'replace')
    report['console_rebuild'] = bytes(m2.console).decode('cp437', 'replace')
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('trace', type=Path)
    parser.add_argument('--build', type=Path, default=ROOT / 'build' / 'game')
    parser.add_argument('--max-steps', type=int, default=100000)
    parser.add_argument('--keep-going', action='store_true')
    parser.add_argument('--report', type=Path)
    parser.add_argument('--self-check', action='store_true',
                        help='compare the original with itself (machine determinism)')
    args = parser.parse_args(argv)
    report = run_lockstep(None if args.self_check else args.build, json.loads(args.trace.read_text()),
                          args.max_steps, args.keep_going)
    text = json.dumps(report, indent=2)
    if args.report:
        args.report.write_text(text + '\n')
    print(text)
    return 0 if report['divergence'] is None else 1


if __name__ == '__main__':
    raise SystemExit(main())
