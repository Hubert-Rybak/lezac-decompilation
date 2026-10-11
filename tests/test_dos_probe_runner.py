"""Contracts for a deliberately limited DOS service fixture, not a full DOS OS."""
import importlib.util
import struct
from pathlib import Path
import pytest

ROOT = Path(__file__).resolve().parents[1]


def module():
    path = ROOT / 'tools/run_dos_probe.py'
    assert path.exists(), 'DOS probe runner has not been implemented'
    spec = importlib.util.spec_from_file_location('dos_probe_runner', path)
    assert spec and spec.loader
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result


def mz(code):
    length = 64 + len(code)
    header = struct.pack('<14H', 0x5a4d, length % 512, (length + 511) // 512,
                         0, 4, 32, 0xffff, 0x100, 0x100, 0, 0, 0, 28, 0)
    return header.ljust(64, b'\x00') + code


def test_synthetic_dos_exit_is_executed_not_invented():
    result = module().run_mz(mz(bytes.fromhex('b8 07 4c cd 21')))
    assert result['exit_code'] == 7
    assert result['stdout'] == ''
    assert result['dos_functions'] == {'4c': 1}


def test_unknown_service_fails_closed():
    with pytest.raises(NotImplementedError):
        module().run_mz(mz(bytes.fromhex('b8 00 3d cd 21')))


def test_instruction_budget_does_not_report_success():
    with pytest.raises(RuntimeError, match='terminate'):
        module().run_mz(mz(bytes.fromhex('eb fe')), instruction_limit=100)


@pytest.mark.parametrize('data', [b'', b'not-an-executable', b'MZ' + bytes(62)])
def test_malformed_mz_is_rejected(data):
    with pytest.raises(ValueError):
        module().run_mz(data)


def test_console_output_comes_from_executed_guest_write():
    code = bytearray.fromhex('0e 1f ba 00 00 b9 03 00 bb 01 00 b4 40 cd 21 b8 00 4c cd 21')
    struct.pack_into('<H', code, 3, len(code))
    result = module().run_mz(mz(bytes(code) + b'OK\n'))
    assert result['exit_code'] == 0
    assert result['stdout'] == 'OK\n'
    assert result['dos_functions'] == {'40': 1, '4c': 1}
