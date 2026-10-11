"""Synthetic archive fixtures exercise the importer; no game bytes are embedded."""
import hashlib
import importlib.util
import io
import zipfile
from pathlib import Path
import pytest

ROOT = Path(__file__).resolve().parents[1]


def load():
    path = ROOT / 'tools/fetch_original.py'
    assert path.exists(), 'The verified original-file importer has not been implemented'
    spec = importlib.util.spec_from_file_location('fetch_original', path)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def fixture(members):
    stream = io.BytesIO()
    with zipfile.ZipFile(stream, 'w') as archive:
        for name, data in members:
            archive.writestr(name, data)
    raw = stream.getvalue()
    manifest = {
        'archive': {'size': len(raw), 'sha256': hashlib.sha256(raw).hexdigest()},
        'files': [{'name': name, 'size': len(data), 'sha256': hashlib.sha256(data).hexdigest()} for name, data in members]
    }
    return raw, manifest


def test_archive_is_authenticated_before_extraction():
    module = load()
    raw, manifest = fixture([('TEST.EXE', b'synthetic-data')])
    assert module.validate_archive(raw, manifest) == {'TEST.EXE': b'synthetic-data'}
    with pytest.raises(ValueError):
        module.validate_archive(raw + b'!', manifest)
    manifest['files'][0]['sha256'] = '0' * 64
    with pytest.raises(ValueError):
        module.validate_archive(raw, manifest)


@pytest.mark.parametrize('name', ['../escape', '/absolute', 'sub/file', 'sub\\file', '..'])
def test_traversal_names_are_rejected(name):
    module = load()
    raw, manifest = fixture([(name, b'x')])
    with pytest.raises(ValueError):
        module.validate_archive(raw, manifest)


def test_unexpected_members_are_rejected():
    module = load()
    raw, manifest = fixture([('A', b'a'), ('B', b'b')])
    manifest['files'].pop()
    with pytest.raises(ValueError):
        module.validate_archive(raw, manifest)


def test_existing_different_files_are_preserved(tmp_path):
    module = load()
    destination = tmp_path / 'game'
    destination.mkdir()
    (destination / 'A').write_bytes(b'own copy')
    with pytest.raises(FileExistsError):
        module.install_files(destination, {'A': b'new'})
    assert (destination / 'A').read_bytes() == b'own copy'
    module.install_files(destination, {'A': b'own copy', 'B': b'new'})
    assert (destination / 'B').read_bytes() == b'new'


def test_existing_symlink_is_not_followed(tmp_path):
    module = load()
    destination = tmp_path / 'game'
    destination.mkdir()
    victim = tmp_path / 'victim'
    victim.write_bytes(b'original')
    (destination / 'A').symlink_to(victim)
    with pytest.raises(FileExistsError):
        module.install_files(destination, {'A': b'original'})
    assert victim.read_bytes() == b'original'
