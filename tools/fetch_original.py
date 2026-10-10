#!/usr/bin/env python3
"""Fetch the pinned shareware archive; verify every byte before extracting.

Original software is deliberately not committed to the repository. This tool
only imports the exact archive described in analysis/provenance.json.
"""
import argparse
import hashlib
import io
import json
import re
import time
import urllib.error
import urllib.request
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def validate_archive(data, manifest):
    archive_info = manifest['archive']
    if len(data) != archive_info['size'] or hashlib.sha256(data).hexdigest() != archive_info['sha256']:
        raise ValueError('Archive size or SHA-256 mismatch')
    expected = {item['name']: item for item in manifest['files']}
    if len(expected) != len(manifest['files']):
        raise ValueError('Duplicate manifest filenames')
    result = {}
    with zipfile.ZipFile(io.BytesIO(data)) as archive:
        members = archive.infolist()
        if len(members) != len(expected) or {i.filename for i in members} != set(expected):
            raise ValueError('Unexpected or duplicate archive members')
        for info in members:
            name = info.filename
            if not re.fullmatch(r'[A-Za-z0-9_]+(?:\.[A-Za-z0-9_]+)?', name):
                raise ValueError(f'Unsafe archive member: {name!r}')
            metadata = expected[name]
            if info.file_size != metadata['size']:
                raise ValueError(f'Member size mismatch: {name}')
            content = archive.read(info)  # ZIP CRC is checked too.
            if hashlib.sha256(content).hexdigest() != metadata['sha256']:
                raise ValueError(f'Member digest mismatch: {name}')
            result[name] = content
    return result


def install_files(destination, files):
    destination = Path(destination)
    if destination.is_symlink():
        raise FileExistsError(f'Refusing symlink directory: {destination}')
    destination.mkdir(parents=True, exist_ok=True)
    for name, data in files.items():
        if not re.fullmatch(r'[A-Za-z0-9_]+(?:\.[A-Za-z0-9_]+)?', name):
            raise ValueError('Unsafe output name')
        path = destination / name
        if path.is_symlink() or (path.exists() and (not path.is_file() or path.read_bytes() != data)):
            raise FileExistsError(f'Refusing to overwrite differing file: {path}')
    for name, data in files.items():
        path = destination / name
        if not path.exists():
            with path.open('xb') as output:
                output.write(data)


def download(url, size):
    for attempt in range(3):
        try:
            with urllib.request.urlopen(url, timeout=45) as response:
                data = response.read(size + 1)
            if len(data) != size:
                raise ValueError('Unexpected archive download size')
            return data
        except (urllib.error.URLError, TimeoutError):
            if attempt == 2:
                raise
            time.sleep(2 ** attempt)
    raise RuntimeError('Download attempts exhausted')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', type=Path, help='Import a local copy instead of downloading')
    args = parser.parse_args()
    manifest = json.loads((ROOT / 'analysis/provenance.json').read_text())
    data = args.archive.read_bytes() if args.archive else download(manifest['archive']['url'], manifest['archive']['size'])
    files = validate_archive(data, manifest)
    install_files(ROOT / 'original/game', files)
    print(f'Verified and imported {len(files)} original files; LEZAC.EXE SHA-256: {hashlib.sha256(files["LEZAC.EXE"]).hexdigest()}')


if __name__ == '__main__':
    main()
