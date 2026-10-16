"""The game executable is compiled from the Pascal sources only, with provenance.

These tests build pascal/LEZAC.PAS with tools/build_game.py (Free Pascal 3.2.2,
i8086-msdos, large model) and check the manifest, reproducibility, the
failure behaviour of the staged build and that no original executable bytes
are carried into the output.
"""
import hashlib
import importlib.util
import json
import shutil
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[1]
ORIGINAL = ROOT / 'original/game/LEZAC.EXE'


def module():
    spec = importlib.util.spec_from_file_location('build_game', ROOT / 'tools/build_game.py')
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result


@pytest.fixture(scope='module')
def built(tmp_path_factory):
    m = module()
    try:
        m.find_compiler()
    except m.BuildError as error:
        pytest.skip(f'i8086 compiler with 8086-large units unavailable: {error}')
    output = tmp_path_factory.mktemp('game') / 'game'
    manifest = m.build(output)
    return m, output, manifest


def sha256(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def test_manifest_records_sources_toolchain_and_outputs(built):
    m, output, manifest = built
    assert manifest == json.loads((output / 'manifest.json').read_text())
    assert manifest['format'] == 'lezac-game-build/1'
    files = manifest['source']['files']
    assert set(files) == {f'pascal/{name}' for name in m.SOURCES}
    for name, digest in files.items():
        assert sha256(ROOT / name) == digest
    toolchain = manifest['toolchain']
    assert toolchain['version'] == '3.2.2'
    assert toolchain['target'] == 'i8086-msdos' and toolchain['memory_model'] == 'large'
    assert '-WmLarge' in toolchain['options'] and '-Mtp' in toolchain['options']
    exe = output / 'LEZAC.EXE'
    assert exe.read_bytes()[:2] == b'MZ'
    assert manifest['outputs']['LEZAC.EXE']['sha256'] == sha256(exe)
    assert manifest['outputs']['LEZAC.MAP']['sha256'] == sha256(output / 'LEZAC.MAP')
    for name, digest in manifest['outputs']['objects'].items():
        assert sha256(output / 'obj' / name) == digest
    assert manifest['inputs_from_original'] == []
    assert sorted(p.name for p in output.iterdir()) == ['LEZAC.EXE', 'LEZAC.MAP', 'compile.log',
                                                        'manifest.json', 'obj']


def test_build_is_reproducible(built, tmp_path):
    m, output, manifest = built
    again = m.build(tmp_path / 'again')
    assert again['outputs']['LEZAC.EXE'] == manifest['outputs']['LEZAC.EXE']


def test_failed_build_keeps_the_previous_output(built, tmp_path):
    m, output, manifest = built
    previous = tmp_path / 'game'
    shutil.copytree(output, previous)
    before = {p.relative_to(previous): p.read_bytes() for p in previous.rglob('*') if p.is_file()}
    broken = tmp_path / 'src'
    shutil.copytree(ROOT / 'pascal', broken)
    (broken / 'LZSYS.INC').write_text((broken / 'LZSYS.INC').read_text() + '\nthis is not Pascal;\n')
    with pytest.raises(m.BuildError, match='Compilation failed'):
        m.build(previous, source_dir=broken)
    after = {p.relative_to(previous): p.read_bytes() for p in previous.rglob('*') if p.is_file()}
    assert after == before
    assert not [p for p in tmp_path.iterdir() if p.name.startswith('.game-')]


def test_missing_source_is_rejected_before_compiling(built, tmp_path):
    m, _output, _manifest = built
    partial = tmp_path / 'src'
    shutil.copytree(ROOT / 'pascal', partial)
    (partial / 'LZACTOR.INC').unlink()
    with pytest.raises(m.BuildError, match='Missing sources'):
        m.build(tmp_path / 'out', source_dir=partial)
    assert not (tmp_path / 'out').exists()


def test_rebuild_is_a_fresh_compilation_not_the_original(built):
    if not ORIGINAL.is_file():
        pytest.skip('original/game not imported (tools/fetch_original.py)')
    _m, output, _manifest = built
    rebuilt = (output / 'LEZAC.EXE').read_bytes()
    original = ORIGINAL.read_bytes()
    assert rebuilt != original
    # No 64-byte run of the original MAIN PROGRAM code (load image 0..0x82CF,
    # file offset 0x770) occurs in the rebuild. The data/text tables and the
    # unit code are excluded: tables are data, and the graphics unit is
    # re-assembled from the same instruction sequence.
    code = original[0x770:0x770 + 0x82D0]
    windows = {rebuilt[i:i + 64] for i in range(len(rebuilt) - 63)}
    shared = [i for i in range(0, len(code) - 63) if code[i:i + 64] in windows]
    assert shared == []
