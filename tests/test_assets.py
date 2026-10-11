"""Synthetic format contracts plus optional, read-only specimen regressions."""
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import subprocess
import sys

import pytest

ROOT = Path(__file__).resolve().parents[1]
GAME = ROOT / "original/game"


def load(name="asset_formats"):
    path = ROOT / "tools" / f"{name}.py"
    assert path.is_file(), f"Missing resource implementation: {path.name}"
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def level_blob(a=b"\x00AB", b=b"\x11CD", width=2, height=1):
    return (struct.pack("<HHBHB", width, height, 17, 9, 0xFE)
            + struct.pack("<H", len(a)) + a
            + struct.pack("<H", len(b)) + b
            + struct.pack("<HH", 0xABCD, 0x1234)
            + b"\x01" + bytes(range(30))
            + b"\x01" + bytes(range(7))
            + b"\x01" + bytes(range(14)))


def encode_level(row):
    return (struct.pack("<HHBHB", row["width"], row["height"],
                        row["counted_tile_id"], row["required_count"],
                        row["opaque_header_byte"])
            + b"".join(struct.pack("<H", len(p["compressed"])) + p["compressed"]
                       for p in row["planes"])
            + struct.pack("<HH", *row["opaque_words"])
            + b"".join(bytes([len(t["records"])]) + b"".join(t["records"])
                       for t in row["trailers"]))


def test_sprite_variable_sizes_preserve_indices_and_offsets():
    rows = load().parse_spr(b"\x02\x02\x01\x00\xff\x01\x01\x07")
    assert rows == [dict(index=0, offset=1, width=2, height=1, pixels=b"\x00\xff"),
                    dict(index=1, offset=5, width=1, height=1, pixels=b"\x07")]
    assert load().parse_spr(b"\x00") == []


@pytest.mark.parametrize("raw", [b"", b"\x01", b"\x01\x01", b"\x01\x02\x01\x07",
                                 b"\x00x", b"\x01\x00\x01", b"\x01\x01\x00"])
def test_sprite_rejects_truncation_trailing_bytes_and_zero_dimensions(raw):
    with pytest.raises(ValueError):
        load().parse_spr(raw)


def test_palette_preserves_rgb6_and_converts_only_on_request():
    m = load()
    raw = bytes(range(64)) * 12
    entries = m.parse_pal(raw)
    assert len(entries) == 256
    assert bytes(v for entry in entries for v in entry) == raw
    assert m.rgb6_to_rgb8((0, 42, 63)) == (0, 170, 255)


@pytest.mark.parametrize("raw", [b"", bytes(767), bytes(769), b"\x40" + bytes(767)],
                         ids=["empty", "short", "long", "component-64"])
def test_palette_rejects_bad_length_or_non_rgb6(raw):
    with pytest.raises(ValueError):
        load().parse_pal(raw)


@pytest.mark.parametrize("entry", [(), (0, 0), (0, 0, 0, 0), (-1, 0, 0), (64, 0, 0), (1.5, 0, 0)])
def test_rgb6_conversion_validates_input(entry):
    with pytest.raises(ValueError):
        load().rgb6_to_rgb8(entry)


def test_scores_preserve_fixed_storage_without_guessing_text_encoding():
    rows = load().parse_scores(b"\x01" + struct.pack("<IB", 0xFFFFFFFF, 2) + b"\x80 Zabc\x00\xff")
    assert rows == [dict(offset=1, score_raw_u32=0xFFFFFFFF, name_length=2,
                         name_storage=b"\x80 Zabc\x00\xff", name_bytes=b"\x80 ")]
    assert load().parse_scores(b"\x00") == []


@pytest.mark.parametrize("raw", [b"", b"\x00x", b"\x01" + bytes(12),
                                 b"\x01" + struct.pack("<IB", 5, 9) + bytes(8)])
def test_scores_reject_truncation_trailing_bytes_and_invalid_length(raw):
    with pytest.raises(ValueError):
        load().parse_scores(raw)


def test_sound_records_preserve_opaque_tails_and_stop_parameter():
    raw = struct.pack("<H", 2) + struct.pack("<HBB", 247, 1, 2) + b"\xaa\xbb"
    raw += struct.pack("<HBB", 30000, 3, 3) + b"\xcc\xdd"
    rows = load().parse_son(raw)
    assert rows == [dict(index=0, offset=2, tone_parameter=247, gate_ticks=1,
                         step_ticks=2, opaque_tail=b"\xaa\xbb", stop=False),
                    dict(index=1, offset=8, tone_parameter=30000, gate_ticks=3,
                         step_ticks=3, opaque_tail=b"\xcc\xdd", stop=True)]
    assert load().parse_son(b"\x00\x00") == []


@pytest.mark.parametrize("raw", [b"", b"\x00", b"\x00\x00x", b"\x01\x00" + bytes(5)])
def test_sounds_reject_bad_framing(raw):
    with pytest.raises(ValueError):
        load().parse_son(raw)


def test_rle_all_controls_use_two_nibbles_plus_one():
    m = load()
    for control in range(256):
        expected = b"A" * ((control >> 4) + 1) + b"B" * ((control & 15) + 1)
        assert m.rle_prefix(bytes([control, 65, 66])) == expected
        assert m.rle_strict(bytes([control, 65, 66]), len(expected)) == expected
    assert m.rle_prefix(b"") == b""
    assert m.rle_strict(b"", 0) == b""


def test_strict_rle_honors_requested_boundary_without_original_extra_store():
    m = load()
    payload = b"\x12AB"
    assert m.rle_strict(payload, 0) == b""
    assert m.rle_strict(payload, 1) == b"A"
    assert m.rle_strict(payload, 2) == b"AA"
    assert m.rle_strict(payload, 5) == b"AABBB"
    with pytest.raises(ValueError, match="underfilled.*5.*6"):
        m.rle_strict(payload, 6)


@pytest.mark.parametrize("raw", [b"\x00", b"\x00A", b"\x00AB\xff"])
def test_rle_rejects_partial_triples_even_after_requested_output(raw):
    m = load()
    with pytest.raises(ValueError):
        m.rle_prefix(raw)
    with pytest.raises(ValueError):
        m.rle_strict(raw, 0)


def test_rle_rejects_negative_required_size():
    with pytest.raises(ValueError):
        load().rle_strict(b"", -1)


def test_levels_round_trip_unknown_fields_and_record_groups():
    m = load()
    raw = level_blob()
    rows = m.parse_levels(raw * 2)
    assert [r["offset"] for r in rows] == [0, len(raw)]
    assert [r["end"] for r in rows] == [len(raw), len(raw) * 2]
    assert rows[0]["opaque_header_byte"] == 0xFE
    assert rows[0]["opaque_words"] == (0xABCD, 0x1234)
    assert rows[0]["words_offset"] == 18
    assert [t["stride"] for t in rows[0]["trailers"]] == [30, 7, 14]
    assert [t["count_offset"] for t in rows[0]["trailers"]] == [22, 53, 61]
    assert [p["prefix"] for p in rows[0]["planes"]] == [b"AB", b"CCDD"]
    assert all(p["missing"] == p["excess"] == 0 for p in rows[0]["planes"])
    assert b"".join(map(encode_level, rows)) == raw * 2
    assert m.parse_levels(b"") == []


def test_levels_report_underfill_without_guessing_or_padding():
    m = load()
    row = m.parse_levels(level_blob(width=3))[0]
    assert [(p["prefix"], p["required"], p["missing"]) for p in row["planes"]] == [
        (b"AB", 3, 1), (b"CCDD", 6, 2)]
    for plane in row["planes"]:
        with pytest.raises(ValueError, match="underfilled"):
            m.rle_strict(plane["compressed"], plane["required"])


def test_levels_preserve_excess_logical_output_separately_from_missing():
    row = load().parse_levels(level_blob(a=b"\xffAB"))[0]
    plane = row["planes"][0]
    assert len(plane["prefix"]) == 32
    assert plane["missing"] == 0 and plane["excess"] == 30


def test_levels_reject_every_nonempty_truncation_and_trailing_garbage():
    m = load()
    raw = level_blob()
    for end in range(1, len(raw)):
        with pytest.raises(ValueError):
            m.parse_levels(raw[:end])
    with pytest.raises(ValueError):
        m.parse_levels(raw + b"x")


@pytest.mark.parametrize("raw", [level_blob(width=0), level_blob(height=0),
                                 level_blob(a=b"\x00A"), level_blob(b=b"\x00")])
def test_levels_reject_zero_dimensions_and_partial_triples(raw):
    with pytest.raises(ValueError):
        load().parse_levels(raw)


SPECIMENS = {
    "BOMOMIMK.SPR": "08dfd0e59ba191c52af2529bdfa140fd15a57829f0c6c13bbb78e156769ac0d1",
    "PROVA.SPR": "d6d6b634f8613251f6e82e9a15c5c68e2be437892bfa25c53eb4bcd9094188b7",
    "FONTS.SPR": "f65aa9bbba2a66ee69f1b279a32c448e36012feb53441fbbb8e64e31bee87a0e",
    "BOMPAL.PAL": "f2559164074ca529ba6c275a9831b11c6da9e6317f09b3d9228c299d6f8f8319",
    "RECS.DAT": "fbc54d16834666dfd45915ffc0181ad54c635e0b19f59ca2c1aa77984a011a95",
    "PROEFS.SON": "b5bc702a27ac85554cc76a62fced07567921b07d5c16f2027a62fbc956df701d",
    "LIVELS.SCH": "d8b553871014108515d6af05b0853de8ccc22e4d3e44d63533ba1167fdc8c7b2",
}


@pytest.fixture
def specimens():
    if not all((GAME / name).is_file() for name in SPECIMENS):
        pytest.skip("Optional original/game resources are not installed")
    data = {name: (GAME / name).read_bytes() for name in SPECIMENS}
    for name, raw in data.items():
        assert hashlib.sha256(raw).hexdigest() == SPECIMENS[name], name
    yield data
    assert all((GAME / name).read_bytes() == raw for name, raw in data.items())


def test_real_sprites_palette_scores_and_sounds_round_trip(specimens):
    m = load()
    sprites = {}
    for name, count in [("BOMOMIMK.SPR", 91), ("PROVA.SPR", 91), ("FONTS.SPR", 68)]:
        rows = sprites[name] = m.parse_spr(specimens[name])
        assert len(rows) == count
        assert bytes([count]) + b"".join(bytes([r["width"], r["height"]]) + r["pixels"]
                                        for r in rows) == specimens[name]
    assert sum(map(len, sprites.values())) == 250
    assert all((r["width"], r["height"]) == (1, 1) for r in sprites["PROVA.SPR"][47:53])
    assert sprites["FONTS.SPR"][26]["offset"] == 2653
    assert {v for r in sprites["FONTS.SPR"] for v in r["pixels"]} == {0, 1, 255}
    palette = m.parse_pal(specimens["BOMPAL.PAL"])
    assert len(palette) == 256 and palette[1] == (0, 0, 42) and palette[-1] == (21, 0, 0)
    assert bytes(v for entry in palette for v in entry) == specimens["BOMPAL.PAL"]
    scores = m.parse_scores(specimens["RECS.DAT"])
    assert len(scores) == 7 and all(r["score_raw_u32"] == 10000 for r in scores)
    assert bytes([len(scores)]) + b"".join(struct.pack("<IB", r["score_raw_u32"], r["name_length"])
        + r["name_storage"] for r in scores) == specimens["RECS.DAT"]
    sounds = m.parse_son(specimens["PROEFS.SON"])
    assert len(sounds) == 130
    assert [r["index"] + 1 for r in sounds if r["stop"]] == [5, 8, 18, 26, 33, 36, 39, 45, 49, 53, 61, 86, 105, 120, 130]
    assert struct.pack("<H", len(sounds)) + b"".join(struct.pack("<HBB", r["tone_parameter"],
        r["gate_ticks"], r["step_ticks"]) + r["opaque_tail"] for r in sounds) == specimens["PROEFS.SON"]
    for name, parser in [("BOMOMIMK.SPR", m.parse_spr), ("PROVA.SPR", m.parse_spr),
                         ("FONTS.SPR", m.parse_spr), ("BOMPAL.PAL", m.parse_pal),
                         ("RECS.DAT", m.parse_scores), ("PROEFS.SON", m.parse_son)]:
        for raw in (specimens[name][:-1], specimens[name] + b"x"):
            with pytest.raises(ValueError):
                parser(raw)


def test_real_levels_exact_boundaries_and_all_fourteen_strict_rejections(specimens):
    m = load()
    raw = specimens["LIVELS.SCH"]
    levels = m.parse_levels(raw)
    assert [r["offset"] for r in levels] == [0, 1242, 4923, 10984, 15225, 21546, 36128]
    assert levels[-1]["end"] == len(raw) == 41047
    assert [(r["width"], r["height"]) for r in levels] == [
        (60, 33), (100, 53), (150, 60), (100, 58), (110, 62), (180, 64), (140, 52)]
    assert [tuple(p["missing"] for p in r["planes"]) for r in levels] == [
        (9, 12), (25, 5), (11, 11), (9, 15), (13, 30), (9, 22), (9, 12)]
    assert b"".join(map(encode_level, levels)) == raw
    for row in levels:
        for plane in row["planes"]:
            assert len(plane["prefix"]) + plane["missing"] == plane["required"]
            assert plane["excess"] == 0
            with pytest.raises(ValueError, match="underfilled"):
                m.rle_strict(plane["compressed"], plane["required"])
    with pytest.raises(ValueError):
        m.parse_levels(raw[:-1])


def fake_game(directory):
    directory.mkdir()
    for name in ("BOMOMIMK.SPR", "PROVA.SPR", "FONTS.SPR"):
        (directory / name).write_bytes(b"\x01\x02\x01\x00\xff")
    (directory / "BOMPAL.PAL").write_bytes(bytes(765) + b"\x3f\x00\x00")
    (directory / "RECS.DAT").write_bytes(b"\x01" + struct.pack("<IB", 10, 8) + b"PRIVATE!")
    (directory / "PROEFS.SON").write_bytes(b"\x01\x00" + struct.pack("<HBB", 30000, 1, 1) + b"\xaa\xbb")
    (directory / "LIVELS.SCH").write_bytes(level_blob(width=3))
    return directory


def run_cli(*args):
    assert (ROOT / "tools/inspect_assets.py").is_file(), "Missing local inspection CLI"
    return subprocess.run([sys.executable, str(ROOT / "tools/inspect_assets.py"), *map(str, args)],
                          cwd=ROOT, text=True, capture_output=True)


def test_cli_summary_is_read_only_and_omits_score_names(tmp_path):
    game = fake_game(tmp_path / "game")
    before = {p.name: p.read_bytes() for p in game.iterdir()}
    result = run_cli("--asset-dir", game)
    assert result.returncode == 0, result.stderr
    report = json.loads(result.stdout)
    assert report["sprites_decoded"] == 3
    assert report["sprite_counts"] == {"BOMOMIMK.SPR": 1, "PROVA.SPR": 1, "FONTS.SPR": 1}
    assert report["score_records"] == report["sound_records"] == report["stop_records"] == 1
    assert report["palette_entries"] == 256
    assert report["level_missing_bytes"] == [[1, 2]] and report["underfilled_planes"] == 2
    assert "PRIVATE!" not in result.stdout and "name_storage" not in result.stdout
    assert {p.name: p.read_bytes() for p in game.iterdir()} == before


def test_cli_missing_inputs_fail_cleanly(tmp_path):
    result = run_cli("--asset-dir", tmp_path)
    assert result.returncode == 2 and "error:" in result.stderr.lower()
    assert "Traceback" not in result.stderr and not result.stdout


def test_cli_rejects_preview_outside_build_without_writing(tmp_path):
    game = fake_game(tmp_path / "game")
    target = tmp_path / "outside.png"
    result = run_cli("--asset-dir", game, "--contact-sheet", target)
    assert result.returncode == 2 and "build/" in result.stderr
    assert not target.exists()


def test_preview_enforces_build_boundary_and_keeps_index_255_visible(tmp_path):
    Image = pytest.importorskip("PIL.Image")
    sys.path.insert(0, str(ROOT / "tools"))
    try:
        m = load("inspect_assets")
    finally:
        sys.path.pop(0)
    repo = tmp_path / "repo"
    repo.mkdir()
    output = repo / "build/preview.png"
    sprites = {"SYNTHETIC": [dict(index=0, width=2, height=1, pixels=b"\x00\xff")]}
    palette = [(0, 0, 0)] * 255 + [(63, 0, 0)]
    m.write_contact_sheet(sprites, palette, output, repo_root=repo)
    with Image.open(output) as image:
        assert image.getpixel((4, 4)) == (48, 48, 48)
        assert image.getpixel((5, 4)) == (255, 0, 0)
        assert image.width > 2 and image.height > 1
    with pytest.raises(FileExistsError):
        m.write_contact_sheet(sprites, palette, output, repo_root=repo)
    outside = tmp_path / "outside"
    outside.mkdir()
    (repo / "build/escape").symlink_to(outside, target_is_directory=True)
    with pytest.raises(ValueError, match="build/"):
        m.write_contact_sheet(sprites, palette, repo / "build/escape/preview.png", repo_root=repo)
    assert not (outside / "preview.png").exists()
