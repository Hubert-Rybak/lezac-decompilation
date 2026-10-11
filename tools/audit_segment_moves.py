#!/usr/bin/env python3
"""Audit unprefixed 16-bit register-direct 8C/8E segment MOV presentations.

This narrow byte decoder is independent of REA/Ghidra/Capstone. It does not
claim to validate other opcodes, memory operands, prefixes or pseudocode.
Retained raw evidence is never modified.
"""
import hashlib
import json
from pathlib import Path
import struct

SHA256 = '7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec'


def decode_register_mov(data):
    if len(data) < 2 or data[0] not in (0x8c, 0x8e) or data[1] >> 6 != 3:
        return None
    seg_index, gp_index = (data[1] >> 3) & 7, data[1] & 7
    if seg_index > 5 or (data[0] == 0x8e and seg_index == 1):
        return None  # Reserved selector encoding or prohibited MOV CS,r16.
    segment = ('es', 'cs', 'ss', 'ds', 'fs', 'gs')[seg_index]
    register = ('ax', 'cx', 'dx', 'bx', 'sp', 'bp', 'si', 'di')[gp_index]
    destination, source = ((segment, register) if data[0] == 0x8e
                           else (register, segment))
    return f'mov {destination}, {source}'


def audit(root):
    root = Path(root)
    data = (root / 'original/game/LEZAC.EXE').read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    if digest != SHA256:
        raise ValueError('Unexpected original executable hash')
    header = struct.unpack_from('<H', data, 8)[0] * 16
    seen, mismatches = {}, []
    for path in sorted((root / 'analysis/rea/verified/functions').glob('*.json')):
        dossier = json.loads(path.read_text())['normalized_result']
        for line in dossier['assembly']:
            raw_address, text = line.split(':', 1)
            address = int(raw_address, 16)
            at = address - 0x10000 + header
            expected = decode_register_mov(data[at:at + 2])
            if expected is None:
                continue
            normalized = ' '.join(text.lower().split()).replace(',', ', ')
            normalized = ' '.join(normalized.split())
            if address in seen:
                if seen[address] != normalized:
                    raise ValueError(f'Inconsistent shared instruction {address:#x}')
                continue
            seen[address] = normalized
            if normalized != expected:
                mismatches.append({'address': hex(address), 'file_offset': hex(at),
                                   'bytes': data[at:at + 2].hex(),
                                   'rea_assembly': text.strip(), 'byte_decoded': expected})
    return {'original_sha256': digest,
            'scope': 'unprefixed 16-bit register-direct 8C/8E segment MOV only',
            'checked_register_moves': len(seen),
            'mismatches': sorted(mismatches, key=lambda row: int(row['address'], 16))}


if __name__ == '__main__':
    print(json.dumps(audit(Path(__file__).resolve().parents[1]), indent=2))
