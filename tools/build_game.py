#!/usr/bin/env python3
"""Build the reconstructed game LEZAC.EXE from the Pascal sources only.

Compiles pascal/LEZAC.PAS (and the units/include files it uses) with Free
Pascal 3.2.2 for i8086-msdos, large memory model. Nothing from original/ is
read: the output is a fresh compilation, not a copy or wrapper of the
original executable. The result goes to build/game/ together with the linker
map, the program/unit object files (for symbol addresses) and manifest.json
recording the source revision, every source hash, the compiler identity and
command line, and the output hashes.

The previous build/game is replaced only after a complete, successful build
(staging directory + rename); a failed build leaves it untouched.

Compiler: $FPC_DOS, else ppcross8086 on PATH, else the reference install.
Runtime units: $FPC_DOS_LARGE (a directory containing rtl/ and rtl-console/
for the 8086-large model), else <compiler dir>/units/msdos/8086-large.
"""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SOURCE_DIR = ROOT / 'pascal'
MAIN = 'LEZAC.PAS'
OUTPUT = ROOT / 'build' / 'game'
FORMAT = 'lezac-game-build/1'
CANDIDATES = [Path('/opt/fpc8086/lib/fpc/3.2.2/ppcross8086'),
              Path('/opt/data/tools/fpc-3.2.2-downloads/cross/lib/fpc/3.2.2/ppcross8086')]
UNIT_SUBDIRS = ('rtl', 'rtl-console')
OPTIONS = ['-Mtp', '-Tmsdos', '-WmLarge', '-XX', '-O1', '-B', '-Xm', '-vewn']
# The sources of the game; units used by tests only are not part of it.
SOURCES = ('LEZAC.PAS', 'LZDATA.INC', 'LZTEXTS.INC', 'LZSYS.INC', 'LZTEXT.INC', 'LZMENU.INC',
           'LZWORLD.INC', 'LZDEBRIS.INC', 'LZACTOR.INC', 'LZGRAPH.PAS', 'LZRLE.PAS',
           'LZTYPES.PAS', 'LZTPRT.PAS')
OBJECTS = ('LEZAC.o', 'LZGRAPH.o', 'LZRLE.o', 'LZTYPES.o', 'LZTPRT.o')


class BuildError(RuntimeError):
    pass


def sha256(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def find_compiler():
    chosen = os.environ.get('FPC_DOS') or shutil.which('ppcross8086')
    if not chosen:
        chosen = next((str(c) for c in CANDIDATES if c.is_file()), None)
    if not chosen:
        raise BuildError('No i8086 cross compiler: set FPC_DOS or install ppcross8086')
    compiler = Path(shutil.which(chosen) or chosen).resolve()
    if not compiler.is_file():
        raise BuildError(f'Compiler does not exist: {compiler}')
    units = Path(os.environ.get('FPC_DOS_LARGE') or compiler.parent / 'units/msdos/8086-large')
    dirs = [units / sub for sub in UNIT_SUBDIRS]
    if not (dirs[0] / 'system.ppu').is_file() or not (dirs[1] / 'crt.ppu').is_file():
        raise BuildError(f'8086-large runtime units not found under {units} (set FPC_DOS_LARGE)')
    version = subprocess.check_output([str(compiler), '-iV'], text=True, timeout=30).strip()
    if version != '3.2.2':
        raise BuildError(f'Expected Free Pascal 3.2.2, got {version!r}')
    return compiler, dirs, version


def git_state():
    def git(*args):
        return subprocess.run(['git', *args], cwd=ROOT, capture_output=True, text=True, timeout=30)
    head = git('rev-parse', 'HEAD')
    if head.returncode:
        return {'revision': None, 'dirty_sources': None}
    status = git('status', '--porcelain', '--', 'pascal')
    dirty = sorted(line[3:] for line in status.stdout.splitlines() if line.strip())
    return {'revision': head.stdout.strip(), 'dirty_sources': dirty}


def build(output=OUTPUT, source_dir=SOURCE_DIR):
    output = Path(output).resolve()
    source_dir = Path(source_dir)
    missing = [name for name in SOURCES if not (source_dir / name).is_file()]
    if missing:
        raise BuildError(f'Missing sources: {missing}')
    compiler, unit_dirs, version = find_compiler()
    output.parent.mkdir(parents=True, exist_ok=True)
    staging = Path(tempfile.mkdtemp(prefix='.game-staging-', dir=output.parent))
    try:
        src = staging / 'src'
        src.mkdir()
        for name in SOURCES:
            shutil.copy2(source_dir / name, src / name)
        obj = staging / 'obj'
        obj.mkdir()
        exe = staging / 'LEZAC.EXE'
        command = [str(compiler), '-n', *(f'-Fu{d}' for d in unit_dirs), *OPTIONS,
                   f'-Fu{src}', f'-Fi{src}', f'-FU{obj}', f'-FE{staging}', f'-o{exe}', str(src / MAIN)]
        result = subprocess.run(command, cwd=staging, capture_output=True, text=True, timeout=600)
        log = result.stdout + result.stderr
        (staging / 'compile.log').write_text(log)
        if result.returncode != 0 or not exe.is_file():
            errors = [l for l in log.splitlines() if 'Error' in l or 'Fatal' in l]
            raise BuildError('Compilation failed:\n' + '\n'.join(errors[-40:] or log.splitlines()[-40:]))
        data = exe.read_bytes()
        if data[:2] != b'MZ':
            raise BuildError('Compiler output is not an MZ executable')
        mapfile = staging / 'LEZAC.map'
        if not mapfile.is_file():
            raise BuildError('Linker map missing')
        mapfile.rename(staging / 'LEZAC.MAP')
        objects = {}
        for name in OBJECTS:
            path = obj / name
            if not path.is_file():
                raise BuildError(f'Object file missing: {name}')
            objects[name] = sha256(path)
        for extra in obj.iterdir():
            if extra.name not in OBJECTS:
                extra.unlink()
        shutil.rmtree(src)
        manifest = {
            'format': FORMAT,
            'built_utc': datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ'),
            'source': {**git_state(),
                       'files': {f'pascal/{name}': sha256(source_dir / name) for name in SOURCES}},
            'toolchain': {'compiler': 'Free Pascal', 'version': version, 'target': 'i8086-msdos',
                          'memory_model': 'large', 'compiler_sha256': sha256(compiler),
                          'runtime_units': {f'{d.name}/{f}': sha256(d / f) for d in unit_dirs
                                            for f in sorted(os.listdir(d))
                                            if f in ('system.ppu', 'dos.ppu', 'crt.ppu')},
                          'options': OPTIONS},
            'outputs': {'LEZAC.EXE': {'sha256': hashlib.sha256(data).hexdigest(), 'bytes': len(data)},
                        'LEZAC.MAP': {'sha256': sha256(staging / 'LEZAC.MAP')},
                        'objects': objects},
            'inputs_from_original': [],
        }
        (staging / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
        old = None
        if output.exists():
            old = output.parent / f'.game-old-{os.getpid()}'
            output.rename(old)
        staging.rename(output)
        if old is not None:
            shutil.rmtree(old, ignore_errors=True)
        return manifest
    except BaseException:
        shutil.rmtree(staging, ignore_errors=True)
        raise


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--output', type=Path, default=OUTPUT)
    args = parser.parse_args(argv)
    try:
        manifest = build(args.output)
    except BuildError as error:
        print(f'build_game: {error}', file=sys.stderr)
        return 1
    exe = manifest['outputs']['LEZAC.EXE']
    print(f"LEZAC.EXE {exe['bytes']} bytes sha256 {exe['sha256']}")
    print(f"source revision {manifest['source']['revision']} dirty {manifest['source']['dirty_sources']}")
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
