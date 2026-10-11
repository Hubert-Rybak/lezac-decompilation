import importlib.util
from pathlib import Path
import pytest

ROOT = Path(__file__).resolve().parents[1]


def module():
    path = ROOT / 'tools/summarize_analysis.py'
    assert path.exists(), 'Coverage summarizer not implemented'
    spec = importlib.util.spec_from_file_location('summary', path)
    assert spec and spec.loader
    value = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(value)
    return value


def test_inclusive_range_union_does_not_count_shared_tails_twice():
    coverage = module().covered_bytes
    assert coverage([{'start': '0x10000', 'end': '0x10003'}, {'start': '0x10002', 'end': '0x10005'}]) == set(range(0x10000, 0x10006))
    assert coverage([]) == set()
    with pytest.raises(ValueError):
        coverage([{'start': '0x10005', 'end': '0x10000'}])


def test_real_inventory_summary_matches_every_record():
    summary, rows = module().summarize(ROOT / 'analysis/rea/verified')
    assert summary['discovered_procedures'] == len(rows) == 215
    assert summary['unique_function_body_bytes'] == 40130
    assert summary['main_module_procedures'] == 87
    assert summary['warning_procedures'] == 5
    assert len({r['address'] for r in rows}) == 215
    assert all(row['file_offset'] >= 1904 for row in rows)
    entry = next(r for r in rows if r['address'] == '0x17783')
    assert entry['file_offset'] == 0x7ef3
