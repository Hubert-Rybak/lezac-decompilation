#!/usr/bin/env python3
"""Build reconstructed Pascal using an existing FPC 3.2.2 installation.

Set FPC/FPC_RTL for host builds and FPC_DOS/FPC_DOS_RTL for the i8086 DOS
cross-compiler. System-installed fpc uses its normal configuration. No compiler
is downloaded and no global compiler configuration is changed by this tool.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
REFERENCE_NATIVE = Path('/opt/data/tools/fpc-3.2.2/lib/fpc/3.2.2/ppcx64')
REFERENCE_DOS = Path('/opt/data/tools/fpc-3.2.2-downloads/cross/lib/fpc/3.2.2/ppcross8086')


class ToolchainUnavailable(FileNotFoundError):
    """Only absence is optional; a configured but broken toolchain must fail."""


def toolchain(target):
    if target not in ('native', 'dos'):
        raise ValueError('Target must be native or dos')
    variable, command, fallback = (('FPC', 'fpc', REFERENCE_NATIVE) if target == 'native'
                                    else ('FPC_DOS', 'ppcross8086', REFERENCE_DOS))
    legacy = os.environ.get('FPC8086') if target == 'dos' else None
    selected = os.environ.get(variable) or legacy or shutil.which(command)
    if not selected and fallback.is_file():
        selected = str(fallback)
    if not selected:
        raise ToolchainUnavailable(f'Install {command} or set {variable}; no compiler found')
    found = shutil.which(selected)
    compiler = Path(found or selected).resolve()
    if not compiler.is_file():
        raise FileNotFoundError(f'Compiler does not exist: {compiler}')
    rtl = (os.environ.get('FPC_RTL' if target == 'native' else 'FPC_DOS_RTL')
           or os.environ.get('FPC_UNITS' if target == 'native' else 'FPC8086_UNITS'))
    relative = 'units/x86_64-linux/rtl' if target == 'native' else 'units/msdos/8086-compact/rtl'
    candidate = compiler.parent / relative
    if not rtl and (candidate / 'system.ppu').is_file():
        rtl = str(candidate)
    paths = rtl.split(os.pathsep) if rtl else []
    if paths and not any((Path(path) / 'system.ppu').is_file() for path in paths):
        raise FileNotFoundError(f'Pascal runtime units missing: {rtl}')
    if target == 'dos' and not rtl:
        raise FileNotFoundError('Set FPC_DOS_RTL to the 8086-compact RTL directory')
    flags = ['-n', *('-Fu' + path for path in paths)] if paths else []
    return str(compiler), flags


def build(source, output, target='native'):
    source, output = Path(source).resolve(strict=True), Path(output).resolve()
    if source == output:
        raise ValueError('Output must not replace the Pascal source')
    compiler, flags = toolchain(target)
    version = subprocess.check_output([compiler, '-iV'], text=True, timeout=30).strip()
    if version != '3.2.2':
        raise ValueError(f'Expected tested Free Pascal 3.2.2, got {version!r}')
    output.parent.mkdir(parents=True, exist_ok=True)
    # Keep the previous artifact untouched until a fresh, validated build exists.
    # A sibling staging directory also keeps the final rename on one filesystem.
    with tempfile.TemporaryDirectory(prefix='.pascal-', dir=output.parent) as directory:
        staging = Path(directory)
        staged_output = staging / output.name
        units = staging / ('units-' + target)
        units.mkdir()
        options = ['-Mtp', '-O2', '-B', '-Fu' + str(ROOT / 'pascal'),
                   '-FU' + str(units), '-FE' + str(staging), '-o' + str(staged_output)]
        if target == 'native':
            options.append('-Cg')
        else:
            # Compact preserves 4-byte far data pointers. Small uses 2-byte pointers.
            # -XX is required by the official archive's smartlinked system.a RTL.
            options += ['-Tmsdos', '-WmCompact', '-XX']
        command = [compiler, *flags, *options, str(source)]
        subprocess.run(command, check=True, timeout=120)
        if not staged_output.is_file():
            raise RuntimeError(f'Compiler did not emit the expected {target} executable/library')
        data = staged_output.read_bytes()
        magic = b'MZ' if target == 'dos' else b'\x7fELF'
        if not data.startswith(magic):
            raise RuntimeError(f'Compiler output is not the expected {target} executable/library')
        result = {'source': str(source), 'output': str(output), 'target': target,
                  'compiler': compiler, 'compiler_version': version,
                  'memory_model': 'compact' if target == 'dos' else 'host-native',
                  'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest(),
                  'command': command}
        os.replace(staged_output, output)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--target', choices=['native', 'dos'], default='native')
    args = parser.parse_args()
    print(json.dumps(build(args.source, args.output, args.target), indent=2))


if __name__ == '__main__':
    main()
