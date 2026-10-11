"""Pascal reconstruction of the original unsafe RLE, in guarded test arenas."""
import ctypes
import importlib.util
import os
from pathlib import Path
import shutil
import subprocess
import pytest
from test_rle_machine import original_rle, plane, SHA256
import hashlib

ROOT = Path(__file__).resolve().parents[1]
DEFAULT = Path('/opt/data/tools/fpc-3.2.2/lib/fpc/3.2.2/ppcx64')


@pytest.fixture(scope='module')
def pascal(tmp_path_factory):
    source = ROOT / 'pascal/LZRLE.PAS'
    bridge = ROOT / 'pascal/LZRLEBR.PAS'
    assert source.exists() and bridge.exists(), 'Pascal RLE reconstruction is missing'
    compiler = os.environ.get('FPC') or shutil.which('fpc') or (str(DEFAULT) if DEFAULT.exists() else None)
    assert compiler, 'Install Free Pascal or set FPC'
    compiler_path = Path(compiler).resolve()
    rtl = os.environ.get('FPC_RTL')
    if not rtl and (compiler_path.parent / 'units/x86_64-linux/rtl/system.ppu').exists():
        rtl = str(compiler_path.parent / 'units/x86_64-linux/rtl')
    work = tmp_path_factory.mktemp('pascal-rle')
    library = work / 'lzrle.so'
    flags = ['-n', '-Fu' + rtl] if rtl else []
    subprocess.run([compiler, *flags, '-Mtp', '-Cg', '-O2', '-Fu' + str(ROOT / 'pascal'),
                    '-FU' + str(work), '-o' + str(library), str(bridge)], check=True)
    lib = ctypes.CDLL(str(library))
    lib.lezac_legacy_rle.argtypes = [ctypes.c_void_p, ctypes.c_uint16, ctypes.c_void_p, ctypes.c_uint16]
    lib.lezac_legacy_rle.restype = None
    return lib


@pytest.fixture(scope='module')
def original():
    data = (ROOT / 'original/game/LEZAC.EXE').read_bytes()
    assert hashlib.sha256(data).hexdigest() == SHA256
    return data


def run_pascal(lib, payload, requested, tail):
    # Entire 64-KiB arenas intentionally make legacy overread/extra stores safe
    # for the host process. Do NOT expose this routine as a safe asset decoder.
    src = (ctypes.c_ubyte * 65536)(*([tail] * 65536))
    dst = (ctypes.c_ubyte * 65536)(*([0xcd] * 65536))
    ctypes.memmove(src, payload, len(payload))
    lib.lezac_legacy_rle(src, len(payload), dst, requested)
    return bytes(dst[:requested + 32])


def test_all_run_controls_and_early_stop_match_original(pascal, original):
    cases = 0
    for control in range(256):
        first, second = (control >> 4) + 1, (control & 15) + 1
        payload = bytes([control, 65, 66])
        for requested in (0, first, first + second - 1, first + second):
            expected, _, _ = original_rle(original, payload, requested, 0)
            assert run_pascal(pascal, payload, requested, 0) == expected, (control, requested)
            cases += 1
    assert cases == 1024


@pytest.mark.parametrize('index', range(7))
@pytest.mark.parametrize('plane_index', range(2))
@pytest.mark.parametrize('tail', [0, 0x55])
def test_pascal_preserves_observed_plane_tail_behavior(pascal, original, index, plane_index, tail):
    payload, required, prefix = plane(original, index, plane_index)
    expected, _, _ = original_rle(original, payload, required, tail)
    assert run_pascal(pascal, payload, required, tail) == expected
    assert expected[:len(prefix)] == prefix
