import hashlib
import importlib.util
import subprocess
from pathlib import Path
import pytest

ROOT = Path(__file__).resolve().parents[1]


def module():
    path = ROOT / 'tools/build_pascal.py'
    assert path.exists(), 'Pascal build driver not implemented'
    spec = importlib.util.spec_from_file_location('pascal_build', path)
    assert spec and spec.loader
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result


def test_build_real_native_tp_program(tmp_path):
    source = tmp_path / 'types.pas'
    source.write_text("program Types; {$mode tp} begin WriteLn(SizeOf(Integer), ',', SizeOf(Word), ',', SizeOf(LongInt)) end.")
    output = tmp_path / 'native/types'
    result = module().build(source, output, target='native')
    assert result['compiler_version'] == '3.2.2'
    assert output.read_bytes().startswith(b'\x7fELF')
    assert subprocess.check_output([str(output)], text=True).strip() == '2,2,4'
    assert not list(tmp_path.glob('*.o'))


def test_invalid_target_is_rejected():
    with pytest.raises(ValueError):
        module().toolchain('invalid')


def test_build_real_dos_compact_program(tmp_path):
    m = module()
    try:
        m.toolchain('dos')
    except m.ToolchainUnavailable:
        pytest.skip('Optional i8086 cross-compiler unavailable')
    source = tmp_path / 'types.pas'
    source.write_text("program Types; {$mode tp} begin WriteLn(SizeOf(Integer), ',', SizeOf(Word), ',', SizeOf(LongInt), ',', SizeOf(Pointer)) end.")
    output = tmp_path / 'dos/TYPES.EXE'
    result = m.build(source, output, target='dos')
    assert result['memory_model'] == 'compact'
    assert output.read_bytes().startswith(b'MZ')
    spec = importlib.util.spec_from_file_location('probe', ROOT / 'tools/run_dos_probe.py')
    assert spec and spec.loader
    probe = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(probe)
    execution = probe.run_mz(output.read_bytes())
    assert execution['exit_code'] == 0
    assert execution['stdout'].strip() == '2,2,4,4'


@pytest.fixture(params=['native', 'dos'])
def built_probe(tmp_path, request):
    m = module()
    target = request.param
    try:
        m.toolchain(target)
    except m.ToolchainUnavailable:
        if target == 'dos':
            pytest.skip('Optional i8086 cross-compiler unavailable')
        raise
    output = tmp_path / target / ('LZPROBE.EXE' if target == 'dos' else 'lzprobe')
    result = m.build(ROOT / 'pascal/LZPROBE.PAS', output, target)
    data = output.read_bytes()
    assert data.startswith(b'MZ' if target == 'dos' else b'\x7fELF')
    assert result['sha256'] == hashlib.sha256(data).hexdigest()
    return m, output, target


def test_unit_after_program_does_not_reuse_previous_executable(built_probe):
    m, output, target = built_probe
    previous = output.read_bytes()
    existing = set(output.parent.iterdir())
    with pytest.raises(RuntimeError, match='did not emit'):
        m.build(ROOT / 'pascal/LZCORE.PAS', output, target)
    assert output.read_bytes() == previous
    assert set(output.parent.iterdir()) == existing


@pytest.mark.parametrize('failure', ['compiler-error', 'timeout', 'invalid-magic'])
def test_failed_build_preserves_previous_executable(built_probe, monkeypatch, failure):
    m, output, target = built_probe
    previous = output.read_bytes()
    existing = set(output.parent.iterdir())
    run = m.subprocess.run

    def fail_compile(command, **kwargs):
        if '-iV' in command:
            return run(command, **kwargs)
        # A compiler/linker can leave a partial or wrong-target output behind.
        artifact = Path(next(arg[2:] for arg in command if arg.startswith('-o')))
        artifact.write_bytes(b'not an executable')
        if failure == 'compiler-error':
            raise subprocess.CalledProcessError(1, command)
        if failure == 'timeout':
            raise subprocess.TimeoutExpired(command, kwargs['timeout'])
        return subprocess.CompletedProcess(command, 0)

    monkeypatch.setattr(m.subprocess, 'run', fail_compile)
    error = {'compiler-error': subprocess.CalledProcessError,
             'timeout': subprocess.TimeoutExpired, 'invalid-magic': RuntimeError}[failure]
    with pytest.raises(error):
        m.build(ROOT / 'pascal/LZPROBE.PAS', output, target)
    assert output.read_bytes() == previous
    assert set(output.parent.iterdir()) == existing


def test_publication_failure_preserves_previous_executable(built_probe, monkeypatch):
    m, output, target = built_probe
    previous = output.read_bytes()
    existing = set(output.parent.iterdir())

    def fail_replace(source, destination):
        assert Path(destination) == output
        assert output.read_bytes() == previous
        raise OSError('publication failed')

    monkeypatch.setattr(m.os, 'replace', fail_replace)
    with pytest.raises(OSError, match='publication failed'):
        m.build(ROOT / 'pascal/LZPROBE.PAS', output, target)
    assert output.read_bytes() == previous
    assert set(output.parent.iterdir()) == existing


def test_successful_rebuild_publishes_fresh_artifact_atomically(built_probe, tmp_path, monkeypatch):
    m, output, target = built_probe
    previous = output.read_bytes()
    existing = set(output.parent.iterdir())
    source = tmp_path / 'replacement.pas'
    source.write_text("program Replacement; begin WriteLn('replacement') end.")
    run, replace = m.subprocess.run, m.os.replace
    compiled, published = [], []

    def observe_compile(command, **kwargs):
        if '-iV' in command:
            return run(command, **kwargs)
        artifact = Path(next(arg[2:] for arg in command if arg.startswith('-o')))
        assert artifact != output
        assert artifact.parent.parent == output.parent
        assert not artifact.exists()
        assert output.read_bytes() == previous
        result = run(command, **kwargs)
        assert output.read_bytes() == previous
        compiled.append(artifact)
        return result

    def observe_replace(source, destination):
        artifact = Path(source)
        assert artifact == compiled[-1]
        assert Path(destination) == output
        assert output.read_bytes() == previous
        data = artifact.read_bytes()
        assert data.startswith(b'MZ' if target == 'dos' else b'\x7fELF')
        published.append(data)
        return replace(source, destination)

    with monkeypatch.context() as patch:
        patch.setattr(m.subprocess, 'run', observe_compile)
        patch.setattr(m.os, 'replace', observe_replace)
        result = m.build(source, output, target)
    data = output.read_bytes()
    assert published == [data]
    assert data != previous
    assert result['source'] == str(source)
    assert result['output'] == str(output)
    assert result['target'] == target
    assert result['bytes'] == len(data)
    assert result['sha256'] == hashlib.sha256(data).hexdigest()
    assert '-o' + str(compiled[0]) in result['command']
    assert not compiled[0].parent.exists()
    assert set(output.parent.iterdir()) == existing
    if target == 'dos':
        spec = importlib.util.spec_from_file_location('probe', ROOT / 'tools/run_dos_probe.py')
        assert spec and spec.loader
        probe = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(probe)
        execution = probe.run_mz(data)
        assert execution['exit_code'] == 0
        assert execution['stdout'].strip() == 'replacement'
    else:
        assert subprocess.check_output([str(output)], text=True).strip() == 'replacement'
