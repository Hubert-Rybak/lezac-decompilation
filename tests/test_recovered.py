"""Differential tests: compiled portable C versus original, relocated DOS bytes."""
import ctypes as C
import itertools
import json
import os
from pathlib import Path
import random
import shlex
import subprocess
import tempfile

import pytest
from dos_machine import DosMachine, ROOT

U8 = C.c_uint8
U16 = C.c_uint16
I16 = C.c_int16
P8 = C.POINTER(U8)
P16 = C.POINTER(U16)


class Collection(C.Structure):
    _fields_ = [("score", U16), ("bonus", U16), ("collected_tile", U8), ("above_present", U8)]


@pytest.fixture(scope="module")
def machine():
    result = DosMachine()
    yield result
    result.verify_unmodified()
    print("\nDOS differential calls:", json.dumps({hex(k): v for k, v in sorted(result.calls.items())}))
    print("Original stack-check executions:", result.stack_checks)


@pytest.fixture(scope="module")
def native():
    source = ROOT / "recovered/lezac.c"
    if not source.is_file():
        yield None  # Fail in each test body, not a fixture/import error (TDD RED).
        return
    scratch = Path(os.environ.get("TMPDIR", str(ROOT.parent / "cache/scratch")))
    scratch.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="lezac-c-", dir=scratch) as directory:
        output = Path(directory) / "liblezac.so"
        command = [os.environ.get("CC", "cc"), "-std=c99", "-Wall", "-Wextra", "-Werror",
                   "-pedantic", "-O2", "-fPIC", "-shared", *shlex.split(os.environ.get("LEZAC_CFLAGS", "")),
                   str(source), "-o", str(output)]
        subprocess.run(command, check=True)
        lib = C.CDLL(str(output))
        lib.lezac_alternating_checksum.argtypes = [U16, P8, U16]
        lib.lezac_alternating_checksum.restype = U16
        lib.lezac_animation_init.argtypes = [P8, U8, U8, U8, U8]
        lib.lezac_animation_init.restype = None
        lib.lezac_wrapping_add_clamp.argtypes = [U8, U8, U8]
        lib.lezac_wrapping_add_clamp.restype = U8
        lib.lezac_friction_step.argtypes = [I16]
        lib.lezac_friction_step.restype = I16
        lib.lezac_state45_init.argtypes = [P8, U16, P16]
        lib.lezac_state45_init.restype = None
        lib.lezac_collect_tile.argtypes = [P8, P16, U16, P16, P16, C.POINTER(Collection)]
        lib.lezac_collect_tile.restype = None
        yield lib


def require_native(native):
    assert native is not None, "Portable C reconstruction is not implemented yet"


def test_original_loader_and_real_stack_check(machine):
    assert machine.header_size == 1904
    assert len(machine.relocations) == 468
    # Actual far call is 1920:04df; Ghidra may display the equivalent 1000:96df.
    assert machine.read(0x1000, 6, 5) == bytes.fromhex("9a df 04 20 19")
    machine.write(0x5000, 0x100, bytes([250]))
    machine.run(0x15715, [100, 10, 0x100, 0x5000])
    assert machine.read(0x5000, 0x100, 1) == bytes([4])
    machine.verify_unmodified()


def test_checksum_differential(machine, native):
    require_native(native)
    rng = random.Random(0x10000)
    cases = [(seed, bytes(data)) for seed in (0, 1, 0x7FFF, 0x8000, 0xFFFF)
             for data in ([], [0], [255], [0, 255], [255, 0], [1, 2, 3], [255] * 256)]
    cases += [(rng.randrange(65536), rng.randbytes(rng.randrange(1025))) for _ in range(256)]
    cases += [(0xFFFF, bytes([255, 0]) * 32767 + b"\xff"),
              (0, bytes([0, 255]) * 32767 + b"\x00")]
    for seed, data in cases:
        # Flat source at offset zero stays within its 64-KiB segment, even at max count.
        machine.write(0x5000, 0, data or b"\x5a")
        machine.write(0x6000, 0xFF, b"\xa5" + seed.to_bytes(2, "little") + b"\x5a")
        machine.run(0x10000, [len(data), 0x100, 0x6000, 0, 0x5000])
        array = (U8 * len(data)).from_buffer_copy(data)
        actual = native.lezac_alternating_checksum(seed, array, len(data))
        assert actual == machine.read_word(0x6000, 0x100), (seed, len(data))
        assert machine.read(0x6000, 0xFF, 1) == b"\xa5"
        assert machine.read(0x6000, 0x102, 1) == b"\x5a"
        assert machine.read(0x5000, 0, len(data)) == data
    assert native.lezac_alternating_checksum(1234, None, 0) == 1234


def test_animation_init_differential(machine, native):
    require_native(native)
    rng = random.Random(0x106AB)
    cases = list(itertools.product((0, 1, 127, 128, 255), repeat=4))
    cases += [tuple(rng.randrange(256) for _ in range(4)) for _ in range(512)]
    for a, b, c, d in cases:
        before = bytes([0xA5]) + rng.randbytes(7) + bytes([0x5A])
        machine.write(0x5000, 0xFF, before)
        machine.run(0x106AB, [d | 0x1200, c | 0x3400, b | 0x5600, a | 0x7800, 0x100, 0x5000])
        array = (U8 * 9).from_buffer_copy(before)
        native.lezac_animation_init(C.cast(C.byref(array, 1), P8), a, b, c, d)
        assert bytes(array) == machine.read(0x5000, 0xFF, 9)
        assert bytes(array)[1:8] == bytes([a, a, b, c, c, d, 1])


def test_wrapping_add_clamp_differential(machine, native):
    require_native(native)
    # Every value/increment pair at one limit, plus every limit at boundary pairs.
    edge = (0, 1, 2, 42, 43, 99, 100, 127, 128, 254, 255)
    cases = itertools.chain(((v, d, 127) for v in range(256) for d in range(256)),
                            ((v, d, limit) for v in edge for d in edge for limit in range(256)))
    for value, delta, limit in cases:
        machine.write(0x5000, 0xFF, bytes([0xA5, value, 0x5A]))
        machine.run(0x15715, [limit | 0xAB00, delta | 0xCD00, 0x100, 0x5000])
        expected = machine.read(0x5000, 0xFF, 3)
        actual = native.lezac_wrapping_add_clamp(value, delta, limit)
        assert expected == bytes([0xA5, actual, 0x5A]), (value, delta, limit)
    assert native.lezac_wrapping_add_clamp(250, 10, 100) == 4


def test_friction_all_int16_values(machine, native):
    require_native(native)
    frame = 0x4000
    for value in range(-32768, 32768):
        machine.write(machine.SS, frame - 13, b"\xa5" + (value & 0xFFFF).to_bytes(2, "little") + b"\x5a")
        machine.run(0x15B86, [frame])
        actual = native.lezac_friction_step(value)
        expected = machine.read(machine.SS, frame - 13, 4)
        assert expected == b"\xa5" + (actual & 0xFFFF).to_bytes(2, "little") + b"\x5a", value
    assert native.lezac_friction_step(-32768) == 0
    assert native.lezac_friction_step(-32767) == -32725


def test_state45_differential(machine, native):
    require_native(native)
    rng = random.Random(0x12E8D)
    fills = [0, 1, 0x00FF, 0xFF00, 0x7FFF, 0x8000, 0xFFFF]
    fills += [rng.randrange(65536) for _ in range(512)]
    for fill in fills:
        before = bytes([0xA5]) + rng.randbytes(45) + bytes([0x5A])
        machine.write(0x5000, 0xFF, before)
        machine.write_word(machine.DS, 0x2070, fill)
        machine.write_word(machine.DS, 0x2082, 0xBEEF)
        machine.run(0x12E8D, [0x1234, 0x100, 0x5000])
        array = (U8 * 47).from_buffer_copy(before)
        index = U16(0xBEEF)
        native.lezac_state45_init(C.cast(C.byref(array, 1), P8), fill, C.byref(index))
        assert bytes(array) == machine.read(0x5000, 0xFF, 47), fill
        assert index.value == machine.read_word(machine.DS, 0x2082) == 9
        assert machine.read_word(machine.DS, 0x2070) == fill


def test_tile_collection_differential(machine, native):
    require_native(native)
    rng = random.Random(0x15AFD)
    current_words = (0, 1, 0x7FFF, 0x8000, 0xFFFF, 0x809A)
    above_words = (0, 1, 0x7FFF, 0x8000, 0xFFFF)
    cases = list(itertools.product(range(256), current_words, above_words))
    cases += [(rng.randrange(256), rng.randrange(65536), rng.randrange(65536)) for _ in range(2048)]
    indexes = (0, 1, 0x20, 0x4000, 0x8000, 0xFFFF)
    for serial, (tile, current, above) in enumerate(cases):
        index = indexes[serial % len(indexes)]
        stride = (2, 40, 0x8000, 0xFFFE)[serial % 4]
        current_offset = (2 * index) & 0xFFFF
        above_offset = (current_offset - stride) & 0xFFFF
        scores = [rng.randrange(65536) for _ in range(12)]
        bonuses = [rng.randrange(65536) for _ in range(12)]
        score = rng.randrange(65536)
        machine.write(machine.DS, 2, b"".join(x.to_bytes(2, "little") for x in scores))
        machine.write(machine.DS, 0x1A, b"".join(x.to_bytes(2, "little") for x in bonuses))
        machine.write_word(machine.DS, 0xC1FE, 0x5000)
        machine.write_word(machine.DS, 0xC1E8, index)
        machine.write_word(machine.DS, 0x206E, 0x6000)
        machine.write_word(machine.DS, 0x2066, stride)
        machine.write_word(machine.DS, 0x2074, score)
        machine.write_word(machine.DS, 0x2072, 0xBEEF)
        machine.write(machine.DS, 0x79A3, b"\xa5")
        machine.write(machine.DS, 0x661E, b"\x5a")
        machine.write(0x5000, index, bytes([tile]))
        machine.write_word(0x6000, current_offset, current)
        machine.write_word(0x6000, above_offset, above)
        machine.run(0x15AFD, [0x2468])
        ctile, ccurrent = U8(tile), U16(current)
        result = Collection(score, 0xBEEF, 0xA5, 0x5A)
        native.lezac_collect_tile(C.byref(ctile), C.byref(ccurrent), above,
                                 (U16 * 12)(*scores), (U16 * 12)(*bonuses), C.byref(result))
        assert ctile.value == machine.read(0x5000, index, 1)[0], (serial, tile, current, above)
        assert ccurrent.value == machine.read_word(0x6000, current_offset)
        assert above == machine.read_word(0x6000, above_offset)
        assert result.score == machine.read_word(machine.DS, 0x2074)
        assert result.bonus == machine.read_word(machine.DS, 0x2072)
        assert result.collected_tile == machine.read(machine.DS, 0x79A3, 1)[0]
        assert result.above_present == machine.read(machine.DS, 0x661E, 1)[0]
