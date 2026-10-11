import importlib.util
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def module():
    path = ROOT / 'tools/audit_segment_moves.py'
    assert path.exists(), 'Segment-register direction audit missing'
    spec = importlib.util.spec_from_file_location('segment_audit', path)
    assert spec and spec.loader
    value = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(value)
    return value


def test_intel_encoding_distinguishes_load_from_store():
    decode = module().decode_register_mov
    assert decode(bytes.fromhex('8e da')) == 'mov ds, dx'
    assert decode(bytes.fromhex('8c da')) == 'mov dx, ds'
    assert decode(bytes.fromhex('8e c0')) == 'mov es, ax'
    assert decode(bytes.fromhex('8c c0')) == 'mov ax, es'
    assert decode(bytes.fromhex('8e 06 00 01')) is None
    assert decode(bytes.fromhex('90')) is None


def test_original_byte_evidence_exposes_known_presentation_errors():
    result = module().audit(ROOT)
    mismatches = {row['address']: row for row in result['mismatches']}
    for address, expected in [('0x19203', 'mov ds, dx'), ('0x19b21', 'mov ds, dx'),
                               ('0x107d2', 'mov es, ax')]:
        assert mismatches[address]['byte_decoded'] == expected
        assert mismatches[address]['rea_assembly'].lower() != expected
    assert result['checked_register_moves'] >= len(mismatches)
    assert result['original_sha256'] == '7579255148c2cb540b26f70dc8181c50b218b6808d8fa5208c832391bafa53ec'
