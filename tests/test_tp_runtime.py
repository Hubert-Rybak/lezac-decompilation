"""Differential tests: LZTPRT (Pascal) versus the ORIGINAL runtime instructions.

LZTPRT reimplements the Turbo Pascal 6 software Real arithmetic, Sin/Cos/Sqrt
and Random that the game executes. Every case calls the original far routine
in Unicorn and the native Pascal build of LZTPRT, and requires identical bits
(or the same runtime error). Inputs are seeded random values plus edge cases
and the exact real operands that appear in the game's code.
"""
import ctypes
import random
import struct
from pathlib import Path

import pytest

from original_runtime import OriginalRuntime
import test_pascal_core  # noqa: F401  (shares the compiler discovery conventions)

ROOT = Path(__file__).resolve().parents[1]

ADD, MUL, DIV, CMP = 0x0EE9, 0x0EFB, 0x0F01, 0x0F0B
FROM_LONG, TRUNC, ROUND = 0x0F0F, 0x0F13, 0x0F1B
INT, FRAC, SQRT, COS, SIN, RANDOM = 0x0F5D, 0x0FAE, 0x0FC2, 0x1021, 0x1034, 0x13A8
RAND_SEED = 0x1AFE


class Real(ctypes.Structure):
    _fields_ = [('w0', ctypes.c_uint16), ('w1', ctypes.c_uint16), ('w2', ctypes.c_uint16)]


def real(t):
    return Real(*t)


@pytest.fixture(scope='module')
def lib(tmp_path_factory):
    import sys
    sys.path.insert(0, str(ROOT / 'tools'))
    import build_pascal
    out = tmp_path_factory.mktemp('tprt') / 'liblztprt.so'
    build_pascal.build(ROOT / 'pascal/LZTPRTBR.PAS', out)
    library = ctypes.CDLL(str(out))
    for name in ('lz_real_add', 'lz_real_mul', 'lz_real_div'):
        getattr(library, name).restype = ctypes.c_int
    library.lz_real_cmp.restype = ctypes.c_int
    library.lz_real_trunc_round.restype = ctypes.c_int
    library.lz_random.restype = ctypes.c_uint16
    library.lz_random.argtypes = [ctypes.c_uint16, ctypes.POINTER(ctypes.c_int32)]
    library.lz_real_from_long.argtypes = [ctypes.c_int32, ctypes.POINTER(Real)]
    return library


@pytest.fixture(scope='module')
def rt():
    runtime = OriginalRuntime()
    yield runtime
    runtime.verify_unmodified()


def regs(a, b=None):
    r = {'ax': a[0], 'bx': a[1], 'dx': a[2]}
    if b is not None:
        r.update({'cx': b[0], 'si': b[1], 'di': b[2]})
    return r


def result(out):
    return (out['ax'], out['bx'], out['dx'])


def from_float(value):
    """Nearest Real48 bit pattern for a Python float (truncated mantissa)."""
    if value == 0:
        return (0, 0, 0)
    sign = 0x8000 if value < 0 else 0
    value = abs(value)
    import math
    m, e = math.frexp(value)          # value = m * 2**e, 0.5 <= m < 1
    exp = e + 128
    mant = int(m * (1 << 40)) & ((1 << 39) - 1)
    w = mant  # 39 bits below the implied one
    low = w & 0xFF
    mid = (w >> 8) & 0xFFFF
    high = ((w >> 24) & 0x7FFF) | sign
    return ((low << 8) | exp, mid, high)


def to_float(t):
    exp = t[0] & 0xFF
    if exp == 0:
        return 0.0
    mant = (t[0] >> 8) | (t[1] << 8) | ((t[2] & 0x7FFF) << 24) | (1 << 39)
    v = mant / float(1 << 40) * 2.0 ** (exp - 128)
    return -v if t[2] & 0x8000 else v


def random_real(rng, exp_lo=0, exp_hi=255):
    exp = rng.randint(exp_lo, exp_hi)
    return ((rng.randrange(256) << 8) | exp, rng.randrange(65536), rng.randrange(65536))


GAME_REALS = [
    (0x0087, 0x0000, 0x4800), (0x297B, 0x8F5C, 0x75C2), (0x0084, 0x0000, 0x0000),
    (0x5C83, 0xC28F, 0x48F5), (0x0088, 0x0000, 0x0000), (0x008D, 0x0000, 0x7A00),
    (0x008B, 0x0000, 0x4800), (0x0088, 0x0000, 0x4800), (0x0089, 0x0000, 0x3400),
    (0x0089, 0x0000, 0x2A00), (0x0086, 0x0000, 0x3400), (0x0089, 0x0000, 0x3280),
    (0x0085, 0x0000, 0x7000), (0x0085, 0x0000, 0x3000), (0x0087, 0x0000, 0x3400),
    (0x0088, 0x0000, 0x3400), (0x0084, 0x0000, 0x7000), (0x0089, 0x0000, 0x0700),
    (0x0089, 0x0000, 0x0C00), (0x2183, 0xDAA2, 0x490F), (0x2181, 0xDAA2, 0xC90F),
    (0x0081, 0x0000, 0x0000), (0x0000, 0x0000, 0x0000),
]


def edge_reals():
    out = list(GAME_REALS)
    for exp in (0, 1, 2, 0x7F, 0x80, 0x81, 0x9E, 0x9F, 0xA0, 0xA7, 0xA8, 0xFE, 0xFF):
        for mant in ((0, 0, 0), (0xFF00, 0xFFFF, 0x7FFF), (0x0100, 0, 0), (0, 0, 0x4000)):
            for sign in (0, 0x8000):
                out.append(((mant[0] & 0xFF00) | exp, mant[1], mant[2] | sign))
    for n in (1, -1, 2, 3, 7, 10, 100, 127, 128, 255, 1000, 65535, -65536, 2**30, -2**31, 2**31 - 1):
        out.append(from_float(float(n)))
    return out


def binary_cases(rng, count):
    edges = edge_reals()
    for a in edges:
        for b in edges[:30]:
            yield a, b
    for _ in range(count):
        if rng.random() < 0.5:
            a = random_real(rng, 0x60, 0xA0)
            b = random_real(rng, 0x60, 0xA0)
        else:
            a = random_real(rng)
            b = random_real(rng)
        if rng.random() < 0.15:
            b = (b[0] & 0xFF00 | (a[0] & 0xFF), b[1], b[2])
        yield a, b


def native_binary(lib, name, a, b):
    r = Real()
    ok = getattr(lib, name)(ctypes.byref(real(a)), ctypes.byref(real(b)), ctypes.byref(r))
    return (r.w0, r.w1, r.w2), ok


@pytest.mark.parametrize('name,offset', [('lz_real_add', ADD), ('lz_real_mul', MUL)])
def test_add_mul_match_original(lib, rt, name, offset):
    rng = random.Random(0x5EED + offset)
    count = 0
    for a, b in binary_cases(rng, 6000):
        out, err = rt.far_call(offset, regs(a, b))
        mine, ok = native_binary(lib, name, a, b)
        if err is not None:
            assert err == 205 and not ok, (name, a, b, err)
        else:
            assert ok and mine == result(out), (name, a, b, result(out), mine)
        count += 1
    assert count > 6000


def test_div_matches_original(lib, rt):
    rng = random.Random(0xD1D)
    count = 0
    for a, b in binary_cases(rng, 6000):
        if b[0] & 0xFF == 0:
            out, err = rt.far_call(DIV, regs(a, b))
            assert err == 200
            continue
        out, err = rt.far_call(DIV, regs(a, b))
        mine, ok = native_binary(lib, 'lz_real_div', a, b)
        if err is not None:
            assert err == 205 and not ok, (a, b, err)
        else:
            assert ok and mine == result(out), (a, b, result(out), mine)
        count += 1
    assert count > 6000


def test_compare_matches_original(lib, rt):
    rng = random.Random(0xC0)
    for a, b in binary_cases(rng, 4000):
        out, err = rt.far_call(CMP, regs(a, b))
        assert err is None
        flags = out['flags']
        expected = -1 if flags & 1 else (0 if flags & 0x40 else 1)
        assert lib.lz_real_cmp(ctypes.byref(real(a)), ctypes.byref(real(b))) == expected, (a, b)


def test_long_to_real_matches_original(lib, rt):
    rng = random.Random(0x10)
    values = [0, 1, -1, 2, -2, 255, 256, 65535, 65536, -65536, 2**31 - 1, -2**31, 0x7FFF, -0x8000]
    values += [rng.randint(-2**31, 2**31 - 1) for _ in range(3000)]
    values += [rng.randint(-70000, 70000) for _ in range(2000)]
    for v in values:
        out, err = rt.far_call(FROM_LONG, {'ax': v & 0xFFFF, 'dx': (v >> 16) & 0xFFFF})
        r = Real()
        lib.lz_real_from_long(v, ctypes.byref(r))
        assert (r.w0, r.w1, r.w2) == result(out), v


@pytest.mark.parametrize('offset,do_round', [(TRUNC, 0), (ROUND, 1)])
def test_trunc_round_match_original(lib, rt, offset, do_round):
    rng = random.Random(0x7A + do_round)
    cases = edge_reals() + [random_real(rng, 0x70, 0xA2) for _ in range(5000)]
    cases += [random_real(rng) for _ in range(1000)]
    for a in cases:
        out, err = rt.far_call(offset, regs(a))
        value = ctypes.c_int32()
        ok = lib.lz_real_trunc_round(ctypes.byref(real(a)), do_round, ctypes.byref(value))
        if err is not None:
            assert err == 207 and not ok, a
        else:
            assert ok, a
            assert value.value & 0xFFFFFFFF == (out['dx'] << 16) | out['ax'], (a, out, value.value)


@pytest.mark.parametrize('name,offset', [('lz_real_int', INT), ('lz_real_frac', FRAC)])
def test_int_frac_match_original(lib, rt, name, offset):
    rng = random.Random(0x1F + offset)
    cases = edge_reals() + [random_real(rng, 0x70, 0xB0) for _ in range(4000)]
    for a in cases:
        out, err = rt.far_call(offset, regs(a))
        assert err is None
        r = Real()
        getattr(lib, name)(ctypes.byref(real(a)), ctypes.byref(r))
        assert (r.w0, r.w1, r.w2) == result(out), (name, a)


def test_sqrt_matches_original(lib, rt):
    rng = random.Random(0x5)
    cases = [c for c in edge_reals() if not c[2] & 0x8000]
    cases += [tuple((w if i < 2 else w & 0x7FFF) for i, w in enumerate(random_real(rng, 1, 254)))
              for _ in range(3000)]
    for n in range(0, 20000, 7):
        cases.append(from_float(float(n)))
    for a in cases:
        out, err = rt.far_call(SQRT, regs(a), limit=2_000_000)
        assert err is None
        r = Real()
        lib.lz_real_sqrt(ctypes.byref(real(a)), ctypes.byref(r))
        assert (r.w0, r.w1, r.w2) == result(out), a


@pytest.mark.parametrize('name,offset', [('lz_real_sin', SIN), ('lz_real_cos', COS)])
def test_sin_cos_match_original(lib, rt, name, offset):
    rng = random.Random(0x51 + offset)
    cases = edge_reals()
    cases += [from_float(i * 6.28 / 128) for i in range(128)]
    cases += [from_float(d / 360 * 6.28) for d in range(-720, 1440, 3)]
    cases += [random_real(rng, 0x60, 0x90) for _ in range(3000)]
    for a in cases:
        if (a[0] & 0xFF) > 0xA8:
            continue
        out, err = rt.far_call(offset, regs(a), limit=2_000_000)
        assert err is None, (name, a, err)
        r = Real()
        getattr(lib, name)(ctypes.byref(real(a)), ctypes.byref(r))
        assert (r.w0, r.w1, r.w2) == result(out), (name, a, to_float(a), result(out), (r.w0, r.w1, r.w2))


def test_random_matches_original(lib, rt):
    rng = random.Random(0xA1)
    seed = 0
    for i in range(6000):
        rng_range = rng.choice([0, 1, 2, 3, 5, 6, 9, 10, 20, 30, 50, 80, 100, 600, 800, 1500, 65535,
                                rng.randrange(65536)])
        rt.write_data(RAND_SEED, struct.pack('<i', seed))
        out, err = rt.far_call(RANDOM, {}, stack_words=[rng_range])
        assert err is None
        original_seed = struct.unpack('<i', rt.read_data(RAND_SEED, 4))[0]
        mine_seed = ctypes.c_int32(seed)
        mine = lib.lz_random(rng_range, ctypes.byref(mine_seed))
        assert mine == out['ax'] and mine_seed.value == original_seed, (i, seed, rng_range)
        seed = original_seed if i % 7 else rng.randint(-2**31, 2**31 - 1)
