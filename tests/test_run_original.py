"""Original-game emulator baseline: run directories, traces and pinned configuration.

Synthetic fixtures exercise the tool without game bytes. The optional emulator
test runs only where the pinned DOSBox-X, Xvfb and the authenticated originals
are present; otherwise it is reported as a skip, not as a pass.
"""
import hashlib
import importlib.util
import json
import shutil
import stat
import struct
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[1]


def load():
    spec = importlib.util.spec_from_file_location('run_original', ROOT / 'tools/run_original.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


ro = load()
PINNED_IDENTITY = {'mismatches': [], 'version_banner': 'synthetic', 'binary_sha256': '0' * 64}


def synthetic_originals(directory, files=None):
    files = files or {'LEZAC.EXE': b'MZ synthetic', 'RECS.DAT': b'\x00score'}
    directory.mkdir(parents=True)
    for name, data in files.items():
        (directory / name).write_bytes(data)
    manifest = {'files': [{'name': name, 'size': len(data), 'sha256': hashlib.sha256(data).hexdigest()}
                          for name, data in files.items()]}
    return files, manifest


def prepare(tmp_path, **kwargs):
    originals = tmp_path / 'original' / 'game'
    files, manifest = synthetic_originals(originals)
    run_dir = ro.prepare_run(tmp_path / 'runs' / 'r1', 'unit', original_dir=originals,
                             identity=kwargs.pop('identity', PINNED_IDENTITY), manifest=manifest, **kwargs)
    return run_dir, originals, files, manifest


def test_originals_are_authenticated_including_unexpected_entries(tmp_path):
    originals = tmp_path / 'game'
    files, manifest = synthetic_originals(originals)
    assert ro.read_verified_originals(originals, manifest) == files
    (originals / 'NEW.DAT').write_bytes(b'left behind by a launch')
    with pytest.raises(ro.RunError, match='unexpected entry'):
        ro.read_verified_originals(originals, manifest)
    (originals / 'NEW.DAT').unlink()
    (originals / 'RECS.DAT').write_bytes(b'\x01score')
    with pytest.raises(ro.RunError, match='mismatch'):
        ro.read_verified_originals(originals, manifest)


def test_symlinked_or_missing_originals_are_rejected(tmp_path):
    originals = tmp_path / 'game'
    files, manifest = synthetic_originals(originals)
    target = tmp_path / 'elsewhere'
    target.write_bytes(files['RECS.DAT'])
    (originals / 'RECS.DAT').unlink()
    (originals / 'RECS.DAT').symlink_to(target)
    with pytest.raises(ro.RunError, match='not a regular file'):
        ro.read_verified_originals(originals, manifest)
    (originals / 'RECS.DAT').unlink()
    with pytest.raises(ro.RunError, match='missing'):
        ro.read_verified_originals(originals, manifest)


def test_prepare_copies_verified_bytes_into_a_private_writable_directory(tmp_path):
    run_dir, originals, files, _ = prepare(tmp_path)
    for name, data in files.items():
        copy = run_dir / 'C' / name
        assert copy.read_bytes() == data
        assert copy.stat().st_mode & stat.S_IWUSR
        assert not copy.samefile(originals / name)
    config = (run_dir / 'dosbox-x.conf').read_text()
    assert f'mount c "{run_dir / "C"}"' in config
    assert str(originals) not in config
    manifest = json.loads((run_dir / 'run.json').read_text())
    assert manifest['format'] == ro.RUN_FORMAT and manifest['sessions'] == []
    assert manifest['pinned_emulator'] is True
    assert {item['name'] for item in manifest['originals']} == set(files)
    assert manifest['config_sha256'] == hashlib.sha256(config.encode()).hexdigest()
    assert manifest['tool_sha256'] == hashlib.sha256((ROOT / 'tools/run_original.py').read_bytes()).hexdigest()


def test_repeated_setup_starts_from_identical_clean_data(tmp_path):
    first, originals, _, manifest = prepare(tmp_path)
    (first / 'C' / 'RECS.DAT').write_bytes(b'played')
    second = ro.prepare_run(tmp_path / 'runs' / 'r2', 'unit', original_dir=originals,
                            identity=PINNED_IDENTITY, manifest=manifest)
    assert ro.read_verified_originals(originals, manifest)
    first_names = sorted(path.name for path in (first / 'C').iterdir())
    assert first_names == sorted(path.name for path in (second / 'C').iterdir())
    assert (second / 'C' / 'RECS.DAT').read_bytes() == (originals / 'RECS.DAT').read_bytes()
    changes = ro.compare_run_copy(first, json.loads((first / 'run.json').read_text())['originals'])
    assert [item['name'] for item in changes['changed']] == ['RECS.DAT']
    assert ro.compare_run_copy(second, json.loads((second / 'run.json').read_text())['originals']) == {
        'changed': [], 'created': [], 'removed': []}


def test_prepare_refuses_reuse_overlap_and_unpinned_emulators(tmp_path):
    run_dir, originals, _, manifest = prepare(tmp_path)
    with pytest.raises(ro.RunError, match='non-empty'):
        ro.prepare_run(run_dir, 'unit', original_dir=originals, identity=PINNED_IDENTITY, manifest=manifest)
    with pytest.raises(ro.RunError, match='separate'):
        ro.prepare_run(originals.parent / 'inside', 'unit', original_dir=originals,
                       identity=PINNED_IDENTITY, manifest=manifest)
    with pytest.raises(ro.RunError, match='separate'):
        ro.prepare_run(tmp_path, 'unit', original_dir=originals, identity=PINNED_IDENTITY, manifest=manifest)
    unpinned = {'mismatches': ['binary SHA-256 differs'], 'version_banner': None}
    with pytest.raises(ro.RunError, match='not the pinned reference'):
        ro.prepare_run(tmp_path / 'runs' / 'r3', 'unit', original_dir=originals, identity=unpinned,
                       manifest=manifest)
    recorded = ro.prepare_run(tmp_path / 'runs' / 'r4', 'unit', original_dir=originals, identity=unpinned,
                              manifest=manifest, allow_unpinned=True)
    assert json.loads((recorded / 'run.json').read_text())['pinned_emulator'] is False
    with pytest.raises(ro.RunError, match='Labels'):
        ro.prepare_run(tmp_path / 'runs' / 'r5', '../x', original_dir=originals, identity=PINNED_IDENTITY,
                       manifest=manifest)


@pytest.mark.parametrize('bad', ['/tmp/a"b', '/tmp/100%', '/tmp/new\nline', 'relative/path'])
def test_config_rejects_paths_that_autoexec_would_misinterpret(bad):
    with pytest.raises(ro.RunError):
        ro.render_config(ro.CONFIG_TEMPLATE.read_text(), Path(bad))


def test_config_template_placeholders_are_all_resolved(tmp_path):
    text = ro.render_config(ro.CONFIG_TEMPLATE.read_text(), tmp_path / 'run')
    assert ro.PLACEHOLDER.search(text) is None
    with pytest.raises(ro.RunError, match='placeholder'):
        ro.render_config('[dosbox]\nmemsize = @UNKNOWN@\n', tmp_path)


def parse_config(text):
    sections, autoexec, section = {}, [], None
    for line in text.splitlines():
        stripped = line.strip()
        if stripped.startswith('[') and stripped.endswith(']'):
            section = stripped[1:-1]
        elif section == 'autoexec':
            autoexec.append(stripped)
        elif stripped and not stripped.startswith('#'):
            key, value = (part.strip() for part in stripped.split('=', 1))
            sections.setdefault(section, {})[key] = value
    return sections, autoexec


def test_pinned_manifest_matches_the_committed_configuration():
    pins = json.loads((ROOT / 'analysis/emulator.json').read_text())
    template = (ROOT / pins['config']['path']).read_bytes()
    assert pins['config']['sha256'] == hashlib.sha256(template).hexdigest()
    sections, autoexec = parse_config(template.decode())
    assert sections['dosbox']['machine'] == pins['config']['machine'] == 'vgaonly'
    assert sections['dosbox']['memsize'] == str(pins['config']['memory_mb'])
    assert {key: sections['cpu'][key] for key in ('core', 'cputype', 'cycles')} == pins['config']['cpu']
    assert sections['dosbox']['saveremark'] == 'false' and sections['dosbox']['synchronize time'] == 'false'
    assert sections['speaker']['pcspeaker'] == 'true' and sections['sblaster']['sbtype'] == 'none'
    assert autoexec[-4:] == ['date 04-23-1996', 'time 12:00:00', 'LEZAC.EXE', 'exit']
    assert autoexec[1] == 'mount c "@GAME_DIR@"'
    assert pins['emulator']['version'] in pins['emulator']['version_banner']
    assert {p['name'] for p in pins['distribution']['packages']} == {'dosbox-x', 'dosbox-x-data'}
    assert all(len(p['sha256']) == 64 for p in pins['distribution']['packages'])
    assert '-defaultmapper' in pins['config']['launch_arguments']


def test_steps_compile_to_absolute_events_and_notes_are_ignored():
    events = ro.compile_trace({'format': ro.TRACE_FORMAT, 'steps': [
        {'wait_ms': 100, 'note': 'menu'}, {'tap': 'z', 'hold_ms': 50}, {'checkpoint': 'cp'},
        {'tap': ['z', 'x'], 'hold_ms': 300}, {'video': 'start'}, {'end': True}]})
    assert events == [
        {'t_ms': 100, 'down': 'z'}, {'t_ms': 150, 'up': 'z'}, {'t_ms': 150, 'checkpoint': 'cp'},
        {'t_ms': 150, 'down': 'z'}, {'t_ms': 150, 'down': 'x'}, {'t_ms': 450, 'up': 'x'},
        {'t_ms': 450, 'up': 'z'}, {'t_ms': 450, 'video': 'start'}, {'t_ms': 450, 'end': True}]


@pytest.mark.parametrize('trace', [
    {'format': 'other', 'events': []},
    {'format': 'lezac-input-trace/1', 'events': [], 'steps': []},
    {'format': 'lezac-input-trace/1', 'events': [{'t_ms': 5, 'down': 'F12'}, {'t_ms': 6, 'up': 'F12'}]},
    {'format': 'lezac-input-trace/1', 'events': [{'t_ms': 5, 'down': 'z'}]},
    {'format': 'lezac-input-trace/1', 'events': [{'t_ms': 5, 'up': 'z'}]},
    {'format': 'lezac-input-trace/1', 'events': [{'t_ms': 5, 'down': 'z'}, {'t_ms': 6, 'down': 'z'}]},
    {'format': 'lezac-input-trace/1', 'events': [{'t_ms': 9, 'checkpoint': 'a'}, {'t_ms': 8, 'checkpoint': 'b'}]},
    {'format': 'lezac-input-trace/1', 'events': [{'t_ms': 1, 'checkpoint': '../escape'}]},
    {'format': 'lezac-input-trace/1', 'events': [{'t_ms': 1, 'down': 'z', 'up': 'z'}]},
    {'format': 'lezac-input-trace/1', 'events': [{'t_ms': 1, 'savestate': True}]},
    {'format': 'lezac-input-trace/1', 'steps': [{'wait_ms': -1}]},
    {'format': 'lezac-input-trace/1', 'steps': [{'tap': 'z', 'extra': 1}]},
])
def test_invalid_traces_are_rejected(trace):
    with pytest.raises(ro.RunError):
        ro.compile_trace(trace)


def test_exported_traces_use_the_unpaused_clock_and_keep_pause_releases():
    log = [
        {'event': 'window', 'game_ms': 0.0},
        {'event': 'key_down', 'key': 'x', 'game_ms': 100.0, 'source': 'input'},
        {'event': 'key_up', 'key': 'x', 'game_ms': 400.0, 'source': 'pause'},
        {'event': 'pause', 'game_ms': 400.0},
        {'event': 'resume', 'game_ms': 400.0},
        {'event': 'checkpoint', 'name': 'after', 'game_ms': 450.0},
        {'event': 'video', 'state': 'start', 'game_ms': 460.0},
        {'event': 'video', 'state': 'stop', 'game_ms': 470.0},
    ]
    trace = ro.export_trace(log, 'unit')
    assert trace['events'] == [
        {'t_ms': 100.0, 'down': 'x'}, {'t_ms': 400.0, 'up': 'x'}, {'t_ms': 450.0, 'checkpoint': 'after'},
        {'t_ms': 460.0, 'video': 'start'}, {'t_ms': 470.0, 'video': 'stop'}, {'t_ms': 470.0, 'end': True}]


def test_xauthority_entry_is_a_private_wildcard_cookie(tmp_path):
    path = tmp_path / 'xauthority'
    ro.write_xauthority(path, b'\x01' * 16)
    data = path.read_bytes()
    family, address, number = struct.unpack('>HHH', data[:6])
    assert (family, address, number) == (0xFFFF, 0, 0)
    assert data[6:8] == struct.pack('>H', 18) and data[8:26] == b'MIT-MAGIC-COOKIE-1'
    assert data[26:28] == struct.pack('>H', 16) and data[28:] == b'\x01' * 16
    assert stat.S_IMODE(path.stat().st_mode) == 0o600
    with pytest.raises(FileExistsError):
        ro.write_xauthority(path, b'\x02' * 16)


def test_committed_traces_are_valid():
    traces = sorted((ROOT / 'emulator/traces').glob('*.json'))
    assert traces
    for path in traces:
        events = ro.compile_trace(json.loads(path.read_text()))
        assert events, path


def test_committed_run_summaries_record_authentic_unchanged_inputs():
    provenance = {item['name']: item['sha256'] for item in json.loads((ROOT / 'analysis/provenance.json').read_text())['files']}
    pins = json.loads((ROOT / 'analysis/emulator.json').read_text())
    summaries = sorted((ROOT / 'analysis/original-runs').glob('*.json'))
    assert summaries
    for path in summaries:
        summary = json.loads(path.read_text())
        assert summary['format'] == 'lezac-original-run-summary/1'
        assert {item['name']: item['sha256'] for item in summary['originals']} == provenance
        assert summary['pinned_emulator'] is True
        assert summary['emulator']['binary_sha256'] == pins['emulator']['binary_sha256']
        assert summary['session']['originals_verified_after_exit'] is True
        assert summary['session']['emulator_returncode'] == 0
        assert '/' not in summary['label'] and 'run_dir' not in summary
        if 'input_trace' in summary:
            trace = ROOT / summary['input_trace']['path']
            assert hashlib.sha256(trace.read_bytes()).hexdigest() == summary['input_trace']['sha256']


def pinned_emulator_available():
    if not (shutil.which('dosbox-x') and shutil.which('Xvfb')):
        return False
    try:
        ro.read_verified_originals()
    except ro.RunError:
        return False
    return not ro.emulator_identity()['mismatches']


@pytest.mark.skipif(not pinned_emulator_available(),
                    reason='pinned DOSBox-X, Xvfb or authenticated original files are not available')
def test_original_boot_frames_match_the_committed_reference(tmp_path):
    reference = json.loads((ROOT / 'analysis/original-runs/boot-smoke.json').read_text())
    run_dir = ro.prepare_run(tmp_path / 'run', 'boot-smoke')
    assert ro.main(['play', str(run_dir), str(ROOT / 'emulator/traces/boot-smoke.json'), '--settle-ms', '500']) == 0
    session = json.loads((run_dir / 'run.json').read_text())['sessions'][-1]
    assert session['originals_verified_after_exit'] is True
    assert session['run_copy_changes'] == {'changed': [], 'created': [], 'removed': []}
    observed = {c['name']: c['frame_sha256'] for c in session['checkpoints']}
    assert observed == {c['name']: c['frame_sha256'] for c in reference['checkpoints']}
