#!/usr/bin/env python3
"""Generate a count-checked index from the complete REA function inventory."""
import argparse
import csv
import json
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def covered_bytes(ranges):
    result = set()
    for item in ranges:
        start, end = int(item['start'], 16), int(item['end'], 16)
        if not 0 <= start <= end <= 0x10FFFF:
            raise ValueError('Invalid inclusive real-mode address range')
        result.update(range(start, end + 1))
    return result


def summarize(directory):
    directory = Path(directory)
    export = json.loads((directory / 'summary.json').read_text())
    overview = json.loads((directory / 'overview.json').read_text())['normalized_result']
    image = json.loads((directory / 'load-image.json').read_text())['normalized_result']
    rows, all_bytes, by_segment, warning_addresses = [], set(), Counter(), []
    if export['failures'] or export['discovered'] != export['exported']:
        raise ValueError('Incomplete REA export')
    for item in export['functions']:
        address = int(item['address'], 16)
        evidence = json.loads((directory / 'functions' / f'{address:x}.json').read_text())
        if evidence['subject']['digest']['sha256'] != export['target_sha256']:
            raise ValueError('Mixed target identities')
        dossier = evidence['normalized_result']
        if dossier['procedure']['address'] != item['address']:
            raise ValueError('Procedure identity mismatch')
        body = dossier['procedure']['body']
        owned = covered_bytes(body['ranges'])
        if len(owned) != body['total_bytes']:
            raise ValueError('Function body byte count mismatch')
        all_bytes.update(owned)
        segment = next(s['name'] for s in overview['segments'] if s['name'] != 'HEADER' and int(s['start'], 16) <= address < int(s['end'], 16))
        by_segment[segment] += 1
        warning = 'WARNING' in dossier['pseudocode']
        if warning:
            warning_addresses.append(item['address'])
        rows.append({
            'address': item['address'], 'ghidra_name': dossier['procedure']['name'],
            'segment': segment,
            'file_offset': address - image['load_segment'] * 16 + image['header_bytes'],
            'body_bytes': len(owned), 'ranges': len(body['ranges']),
            'instructions': len(dossier['assembly']),
            'pseudocode_lines': len(dossier['pseudocode'].splitlines()),
            'decompiler_warning': warning, 'evidence_id': evidence['evidence_id'],
        })
    if len(rows) != export['discovered'] or len({r['address'] for r in rows}) != len(rows):
        raise ValueError('Inventory count or uniqueness mismatch')
    summary = {
        'target_sha256': export['target_sha256'],
        'discovered_procedures': len(rows),
        'main_module_procedures': by_segment['CODE_0'],
        'procedures_by_segment': dict(by_segment),
        'unique_function_body_bytes': len(all_bytes),
        'main_module_function_body_bytes': sum(0x10000 <= a < 0x182D0 for a in all_bytes),
        'warning_procedures': len(warning_addresses),
        'warning_addresses': warning_addresses,
        'load_image_status': image['status'],
        'load_image_checks': len(image['checks']),
        'all_load_image_checks_matched': all(c['matched'] for c in image['checks']),
        'coverage_basis': 'Union of inclusive Ghidra function-body ranges; not proof of exhaustive code discovery or recovered source ownership.',
        'known_vector_targets_not_in_inventory': ['0x10fbe', '0x11091'],
    }
    return summary, rows


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--evidence', type=Path, default=ROOT / 'analysis/rea/verified')
    parser.add_argument('--output', type=Path, default=ROOT / 'analysis')
    args = parser.parse_args()
    summary, rows = summarize(args.evidence)
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output / 'coverage.json').write_text(json.dumps(summary, indent=2) + '\n')
    with (args.output / 'functions.csv').open('w', newline='') as output:
        writer = csv.DictWriter(output, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)
    print(json.dumps(summary, indent=2))


if __name__ == '__main__':
    main()
