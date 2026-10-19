#!/usr/bin/env python3
"""Symbol addresses of a Free Pascal i8086-msdos build from its map and OMF objects.

The internal linker's map lists public code symbols and where every object
file's segments were placed, but not the program's own (local) variables.
Those are LPUBDEF/PUBDEF records in the OMF object files. Combining both gives
load-segment-relative (segment, offset) pairs for every named symbol, e.g.
U_$P$LEZAC_$$_ACTORS or CRT_$$_DELAY$WORD.
"""
import re
import struct
from pathlib import Path


def _index(data, at):
    first = data[at]
    if first & 0x80:
        return ((first & 0x7F) << 8) | data[at + 1], at + 2
    return first, at + 1


def omf_symbols(path):
    """{name: (segment name, class name, offset)} for PUBDEF/LPUBDEF records."""
    data = Path(path).read_bytes()
    names, segments, symbols = [None], [None], {}
    at = 0
    while at + 3 <= len(data):
        kind = data[at]
        length = struct.unpack_from('<H', data, at + 1)[0]
        body = data[at + 3:at + 3 + length - 1]
        at += 3 + length
        if kind == 0x96:                      # LNAMES
            i = 0
            while i < len(body):
                n = body[i]
                names.append(body[i + 1:i + 1 + n].decode('latin-1'))
                i += 1 + n
        elif kind in (0x98, 0x99):            # SEGDEF
            acbp = body[0]
            i = 1
            if acbp >> 5 == 0:
                i += 3
            i += 2 if kind == 0x98 else 4
            seg_name, i = _index(body, i)
            class_name, i = _index(body, i)
            segments.append((names[seg_name], names[class_name]))
        elif kind in (0x90, 0x91, 0xB6, 0xB7):  # PUBDEF / LPUBDEF
            i = 0
            _group, i = _index(body, i)
            segment, i = _index(body, i)
            if segment == 0:
                i += 2
            wide = kind in (0x91, 0xB7)
            while i < len(body):
                n = body[i]
                name = body[i + 1:i + 1 + n].decode('latin-1')
                i += 1 + n
                offset = struct.unpack_from('<I' if wide else '<H', body, i)[0]
                i += 4 if wide else 2
                _type, i = _index(body, i)
                seg = segments[segment] if segment else (None, None)
                symbols[name] = (seg[0], seg[1], offset)
        elif kind in (0x8A, 0x8B):            # MODEND
            break
    return symbols


PLACEMENT = re.compile(r'^\s+(\S+)\|\|(\S+)\s+([0-9A-F]{4}):([0-9A-F]{4})\s+0x([0-9a-f]+)\s+(.+?)\s*$')
PUBLIC = re.compile(r'^\s+([0-9A-F]{4}):([0-9A-F]{4})\s+(\S+)\s*$')


def map_placements(map_path):
    """{(object basename, segment, class): (paragraph, offset)} and public symbols."""
    placements, publics = {}, {}
    for line in Path(map_path).read_text(errors='replace').splitlines():
        m = PLACEMENT.match(line)
        if m:
            seg, cls, para, off, _size, obj = m.groups()
            placements[(Path(obj).name, seg.upper(), cls.upper())] = (int(para, 16), int(off, 16))
            continue
        m = PUBLIC.match(line)
        if m:
            publics[m.group(3)] = (int(m.group(1), 16), int(m.group(2), 16))
    return placements, publics


def symbol_table(map_path, objects):
    """{name: (paragraph, offset)} relative to the load segment of the image."""
    placements, table = map_placements(map_path)
    for obj in objects:
        obj = Path(obj)
        for name, (seg, cls, offset) in omf_symbols(obj).items():
            base = placements.get((obj.name, (seg or '').upper(), (cls or '').upper()))
            if base is None:
                continue
            para, off = base
            linear = para * 16 + off + offset
            table[name] = (linear >> 4, linear & 0xF) if name not in table else table[name]
    return table


def linear(table, name, load_seg):
    para, off = table[name]
    return (load_seg + para) * 16 + off


if __name__ == '__main__':
    import sys
    tab = symbol_table(sys.argv[1], sys.argv[2:])
    for k in sorted(tab):
        print(f'{tab[k][0]:04X}:{tab[k][1]:04X} {k}')
