"""Execute newly compiled DOS/host Pascal and compare with original x86 results."""
import importlib.util
from pathlib import Path
import struct
import subprocess
import pytest
from dos_machine import DosMachine, EXE, ROOT
from test_rle_machine import original_rle


def load(name):
    spec = importlib.util.spec_from_file_location(name, ROOT / 'tools' / (name + '.py'))
    assert spec and spec.loader
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result


def original_outputs():
    m = DosMachine()
    values = {}
    def save(label, data):
        values[label] = bytes(data).hex().upper()
    data = bytes([1, 2, 3, 4, 255, 0, 128])
    m.write(0x5000, 0, data)
    m.write_word(0x6000, 0x100, 0xffff)
    m.run(0x10000, [len(data), 0x100, 0x6000, 0, 0x5000])
    save('CHECKSUM', m.read(0x6000, 0x100, 2))
    m.run(0x106ab, [3, 255, 7, 250, 0x100, 0x5000])
    save('ANIMATION', m.read(0x5000, 0x100, 7))
    m.write(0x5000, 0x100, bytes([250]))
    m.run(0x15715, [100, 10, 0x100, 0x5000])
    save('WRAP', m.read(0x5000, 0x100, 1))
    for i, value in enumerate([-32768, -43, 0, 43, 32767]):
        m.write_word(m.SS, 0x4000 - 12, value)
        m.run(0x15b86, [0x4000])
        save(f'FRICTION{i}', m.read(m.SS, 0x4000 - 12, 2))
    m.write_word(m.DS, 0x2070, 0xff00)
    m.write_word(m.DS, 0x2082, 0xbeef)
    m.run(0x12e8d, [0x1234, 0x100, 0x5000])
    save('STATE45', m.read(0x5000, 0x100, 45))
    save('INDEX', m.read(m.DS, 0x2082, 2))
    m.write(m.DS, 2, struct.pack('<12H', *range(100, 112)))
    m.write(m.DS, 0x1a, struct.pack('<12H', *range(200, 212)))
    for offset, value in [(0xc1fe, 0x5000), (0xc1e8, 2), (0x206e, 0x6000),
                          (0x2066, 2), (0x2074, 65530), (0x2072, 0xbeef)]:
        m.write_word(m.DS, offset, value)
    m.write(0x5000, 2, b'\x67')
    m.write_word(0x6000, 4, 0x8001)
    m.write_word(0x6000, 2, 0x7fff)
    m.run(0x15afd, [0x2468])
    save('COLLECTION', m.read(m.DS, 0x2074, 2) + m.read(m.DS, 0x2072, 2)
         + m.read(m.DS, 0x79a3, 1) + m.read(m.DS, 0x661e, 1))
    save('TILE', m.read(0x5000, 2, 1))
    save('CELL', m.read(0x6000, 4, 2))
    m.verify_unmodified()
    assert sum(m.calls.values()) == 10
    for label, requested in [('RLE', 3), ('RLEZERO', 0)]:
        result, _, _ = original_rle(EXE.read_bytes(), b'\x10AB', requested, 0)
        save(label, result[:16])
    return values


@pytest.mark.parametrize('target', ['native', 'dos'])
def test_executed_pascal_probe_matches_original(tmp_path, target):
    source = ROOT / 'pascal/LZPROBE.PAS'
    assert source.exists(), 'Pascal executable probe not implemented'
    builder = load('build_pascal')
    try:
        builder.toolchain(target)
    except builder.ToolchainUnavailable:
        if target == 'dos':
            pytest.skip('Optional i8086 cross compiler not installed')
        raise
    binary = tmp_path / target / ('LZPROBE.EXE' if target == 'dos' else 'lzprobe')
    builder.build(source, binary, target)
    if target == 'dos':
        execution = load('run_dos_probe').run_mz(binary.read_bytes())
        assert execution['exit_code'] == 0
        text = execution['stdout']
    else:
        text = subprocess.check_output([str(binary)], text=True)
    lines = text.splitlines()
    assert lines[0] == ('TYPES 2 2 4 4' if target == 'dos' else 'TYPES 2 2 4 8')
    actual = dict(line.split(':', 1) for line in lines[1:])
    assert len(actual) == len(lines) - 1
    assert actual == original_outputs()
