#!/usr/bin/env python3
"""Local-only resource summary and opt-in sprite contact sheet under build/."""
import argparse
import json
from pathlib import Path

try:
    from .asset_formats import (parse_levels, parse_pal, parse_scores, parse_son,
                                parse_spr, rgb6_to_rgb8)
except ImportError:  # Direct execution: python tools/inspect_assets.py
    from asset_formats import (parse_levels, parse_pal, parse_scores, parse_son,
                               parse_spr, rgb6_to_rgb8)

ROOT = Path(__file__).resolve().parents[1]
SPRITE_FILES = ("BOMOMIMK.SPR", "PROVA.SPR", "FONTS.SPR")


def write_contact_sheet(sprites, palette, output, *, repo_root=ROOT):
    """Preview raw indexed sprites; index 0 is transparent, 255 is a color.

    A new PNG is created exclusively within the repository's real build/
    directory. Existing files and symlink escapes are refused. Pillow is optional
    for inspection and required only for previews. Font tinting is not applied.
    """
    root = Path(repo_root).resolve()
    build = root / "build"
    output = Path(output)
    if not output.is_absolute():
        output = root / output
    target = output.resolve()
    if build.resolve() != build or not target.is_relative_to(build) or target == build:
        raise ValueError("contact sheet must remain inside repository build/")
    if output.is_symlink() or target.exists():
        raise FileExistsError(f"refusing to overwrite contact sheet: {output}")
    from PIL import Image, ImageDraw

    frames = [(name, frame) for name, group in sprites.items() for frame in group]
    if not frames:
        raise ValueError("no sprites to preview")
    columns = 16
    cell_width = max(108, max(frame["width"] for _, frame in frames) + 8)
    cell_height = max(frame["height"] for _, frame in frames) + 22
    rows = (len(frames) + columns - 1) // columns
    sheet = Image.new("RGB", (columns * cell_width, rows * cell_height), (48, 48, 48))
    draw = ImageDraw.Draw(sheet)
    display_palette = [v for entry in palette for v in rgb6_to_rgb8(entry)]
    for position, (name, frame) in enumerate(frames):
        x, y = (position % columns) * cell_width + 4, (position // columns) * cell_height + 4
        size = (frame["width"], frame["height"])
        image = Image.frombytes("P", size, frame["pixels"])
        image.putpalette(display_palette)
        mask = Image.frombytes("L", size, bytes(255 if v else 0 for v in frame["pixels"]))
        sheet.paste(image.convert("RGB"), (x, y), mask)
        draw.text((x, y + cell_height - 18), f"{Path(name).stem}:{frame['index']}", fill=(220, 220, 220))
    target.parent.mkdir(parents=True, exist_ok=True)
    with target.open("xb") as stream:
        sheet.save(stream, format="PNG")
    return target


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--asset-dir", type=Path, default=ROOT / "original/game",
                        help="local resource directory (default: original/game)")
    parser.add_argument("--contact-sheet", type=Path, metavar="build/FILE.png",
                        help="create a new local-only PNG; never overwrite or write outside build/")
    args = parser.parse_args(argv)
    try:
        sprites = {name: parse_spr((args.asset_dir / name).read_bytes()) for name in SPRITE_FILES}
        palette = parse_pal((args.asset_dir / "BOMPAL.PAL").read_bytes())
        scores = parse_scores((args.asset_dir / "RECS.DAT").read_bytes())
        sounds = parse_son((args.asset_dir / "PROEFS.SON").read_bytes())
        levels = parse_levels((args.asset_dir / "LIVELS.SCH").read_bytes())
        report: dict[str, object] = dict(sprite_counts={name: len(rows) for name, rows in sprites.items()},
                      sprites_decoded=sum(map(len, sprites.values())),
                      palette_entries=len(palette), score_records=len(scores),
                      sound_records=len(sounds), stop_records=sum(row["stop"] for row in sounds),
                      level_records=len(levels),
                      underfilled_planes=sum(p["missing"] > 0 for row in levels for p in row["planes"]),
                      level_missing_bytes=[[p["missing"] for p in row["planes"]] for row in levels])
        if args.contact_sheet is not None:
            path = write_contact_sheet(sprites, palette, args.contact_sheet)
            report["contact_sheet"] = path.relative_to(ROOT).as_posix()
    except (OSError, ValueError, ImportError) as error:
        parser.error(str(error))
    print(json.dumps(report, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
