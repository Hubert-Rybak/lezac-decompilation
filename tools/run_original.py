#!/usr/bin/env python3
"""Run the authenticated ORIGINAL Larax & Zaco under the pinned DOSBox-X baseline.

Each run gets a fresh disposable directory populated from hash-verified copies of
original/game. The analysis inputs are never mounted, and they are verified again
after the emulator exits. See docs/emulator-baseline.md for the workflow.

`prepare --game-build build/game` makes the run use the source-built LEZAC.EXE of
tools/build_game.py instead of the original executable (the data files are still
the authenticated originals); run.json then records the rebuild's hash and
build manifest. See docs/reconstruction.md.

Subcommands:
  verify                         authenticate original/game against provenance
  check-emulator                 compare the installed DOSBox-X with analysis/emulator.json
  prepare [--run-dir DIR]        create a disposable run directory
  launch RUN_DIR                 interactive run on the current X display
  play RUN_DIR TRACE             headless run that replays an input trace
  session-start RUN_DIR          headless run driven by session-send commands
  session-send RUN_DIR CMD...    e.g. "resume" "tap x 400" "checkpoint t1" "pause"
  export-trace RUN_DIR OUT       convert a recorded session into a trace
  summarize RUN_DIR OUT          write a path-free summary of a finished run
"""
import argparse
import ctypes
import ctypes.util
import datetime
import hashlib
import json
import os
import re
import secrets
import shutil
import socket
import struct
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PROVENANCE = ROOT / 'analysis/provenance.json'
EMULATOR_MANIFEST = ROOT / 'analysis/emulator.json'
CONFIG_TEMPLATE = ROOT / 'emulator/dosbox-x-reference.conf'
ORIGINAL_DIR = ROOT / 'original/game'
RUNS_DIR = ROOT / 'build/original-runs'

TRACE_FORMAT = 'lezac-input-trace/1'
RUN_FORMAT = 'lezac-original-run/1'
SAFE_FILE = re.compile(r'[A-Za-z0-9_]+(?:\.[A-Za-z0-9_]+)?')
SAFE_LABEL = re.compile(r'[A-Za-z0-9][A-Za-z0-9_-]{0,63}')
# The mount path is written into AUTOEXEC: no quotes, % expansion or control bytes.
SAFE_MOUNT_PATH = re.compile(r'/[A-Za-z0-9_./+ -]*')
PLACEHOLDER = re.compile(r'@[A-Z_]+@')
# Keys a trace may press: the game's documented controls plus menu keys. Host
# hotkeys (F12 combinations) are issued only by the dedicated commands below.
GAME_KEYS = frozenset([
    'z', 'x', 'c', 'm', 'n', 'Left', 'Right', 'Up', 'Down', '0', 'KP_0', 'KP_Insert',
    '1', '2', 'i', 'l', 'r', 'e', 's', 'space', 'Return', 'Escape',
])
MAX_TRACE_MS = 4 * 60 * 60 * 1000
HOST_KEY = 'F12'


class RunError(RuntimeError):
    pass


def sha256_bytes(data):
    return hashlib.sha256(data).hexdigest()


def sha256_file(path):
    digest = hashlib.sha256()
    with open(path, 'rb') as handle:
        for block in iter(lambda: handle.read(1 << 20), b''):
            digest.update(block)
    return digest.hexdigest()


def utc_now():
    return datetime.datetime.now(datetime.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')


def load_json(path):
    return json.loads(Path(path).read_text())


def write_json(path, value):
    path = Path(path)
    tmp = path.with_name(path.name + '.tmp')
    tmp.write_text(json.dumps(value, indent=2, sort_keys=True) + '\n')
    os.replace(tmp, path)


# --------------------------------------------------------------------------
# Original inputs


def read_verified_originals(directory=ORIGINAL_DIR, manifest=None):
    """Return {name: bytes} for every pinned file, or raise RunError.

    Extra entries are rejected too: a launch inside the analysis directory would
    typically leave a changed score file or new files behind.
    """
    manifest = manifest or load_json(PROVENANCE)
    directory = Path(directory)
    if directory.is_symlink() or not directory.is_dir():
        raise RunError(f'Original directory missing or not a real directory: {directory}')
    expected = {item['name']: item for item in manifest['files']}
    present = {entry.name for entry in directory.iterdir()}
    problems = [f'unexpected entry: {name}' for name in sorted(present - set(expected))]
    files = {}
    for name, item in sorted(expected.items()):
        path = directory / name
        if path.is_symlink() or not path.is_file():
            problems.append(f'missing or not a regular file: {name}')
            continue
        data = path.read_bytes()
        if len(data) != item['size'] or sha256_bytes(data) != item['sha256']:
            problems.append(f'size or SHA-256 mismatch: {name}')
            continue
        files[name] = data
    if problems:
        raise RunError('Original inputs failed authentication: ' + '; '.join(problems))
    return files


def originals_summary(files):
    return [{'name': name, 'size': len(data), 'sha256': sha256_bytes(data)}
            for name, data in sorted(files.items())]


# --------------------------------------------------------------------------
# Emulator identity


def emulator_identity(binary=None, manifest=None):
    """Describe the installed emulator and list deviations from the pin."""
    manifest = manifest or load_json(EMULATOR_MANIFEST)
    pinned = manifest['emulator']
    binary = binary or shutil.which('dosbox-x')
    identity = {'binary': binary, 'mismatches': []}
    if not binary:
        identity['mismatches'].append('dosbox-x not found on PATH')
        return identity
    resolved = Path(binary).resolve()
    identity['resolved_binary'] = str(resolved)
    identity['binary_sha256'] = sha256_file(resolved)
    if identity['binary_sha256'] != pinned['binary_sha256']:
        identity['mismatches'].append('binary SHA-256 differs from analysis/emulator.json')
    try:
        result = subprocess.run([str(resolved), '-version'], capture_output=True, text=True,
                                timeout=30, env={'PATH': os.environ.get('PATH', ''), 'HOME': '/nonexistent'})
        banner = next((line.strip() for line in (result.stdout + result.stderr).splitlines()
                       if line.startswith('DOSBox-X version')), None)
    except (OSError, subprocess.TimeoutExpired) as error:
        banner = None
        identity['version_error'] = str(error)
    identity['version_banner'] = banner
    if banner != pinned['version_banner']:
        identity['mismatches'].append(f'version banner {banner!r} differs from pinned {pinned["version_banner"]!r}')
    packages = {}
    if shutil.which('dpkg-query'):
        for package in [p['name'] for p in manifest['distribution']['packages']] + ['libsdl2-2.0-0', 'xvfb']:
            result = subprocess.run(['dpkg-query', '-W', '-f=${Version}', package],
                                    capture_output=True, text=True)
            packages[package] = result.stdout.strip() if result.returncode == 0 else None
        for package in manifest['distribution']['packages']:
            if packages.get(package['name']) != package['version']:
                identity['mismatches'].append(f'package {package["name"]} is {packages.get(package["name"])!r}, '
                                              f'pinned {package["version"]!r}')
    identity['packages'] = packages
    return identity


# --------------------------------------------------------------------------
# Run directories


RUN_DEFINITION_PATHS = ('tools/run_original.py', 'emulator', 'analysis/emulator.json', 'analysis/provenance.json')


def git_revision():
    try:
        head = subprocess.run(['git', 'rev-parse', 'HEAD'], cwd=ROOT, capture_output=True, text=True,
                              check=True).stdout.strip()
        dirty = subprocess.run(['git', 'status', '--porcelain', '--untracked-files=no'], cwd=ROOT,
                               capture_output=True, text=True, check=True).stdout.strip() != ''
        # Uncommitted or untracked files that define this run (tool, config, traces, pins).
        definition = subprocess.run(['git', 'status', '--porcelain', '--untracked-files=all', '--',
                                     *RUN_DEFINITION_PATHS], cwd=ROOT, capture_output=True, text=True,
                                    check=True).stdout.splitlines()
        return {'commit': head, 'tracked_changes': dirty, 'uncommitted_run_definition': definition}
    except (OSError, subprocess.CalledProcessError):
        return {'commit': None, 'tracked_changes': None, 'uncommitted_run_definition': None}


def render_config(template, run_dir):
    run_dir = Path(run_dir)
    values = {
        '@GAME_DIR@': run_dir / 'C',
        '@CAPTURE_DIR@': run_dir / 'capture',
        '@LOG_FILE@': run_dir / 'dosbox-x.log',
        # Deliberately absent: the built-in default bindings (-defaultmapper) apply.
        '@MAPPER_FILE@': run_dir / 'home' / 'no-mapper-file.map',
    }
    text = template
    for key, value in values.items():
        value = str(value)
        if not SAFE_MOUNT_PATH.fullmatch(value):
            raise RunError(f'Unsupported characters in run path for DOSBox-X configuration: {value!r}')
        text = text.replace(key, value)
    leftover = PLACEHOLDER.search(text)
    if leftover:
        raise RunError(f'Unresolved configuration placeholder {leftover.group()}')
    return text


def is_within(path, parent):
    try:
        Path(path).resolve().relative_to(Path(parent).resolve())
        return True
    except ValueError:
        return False


def default_run_dir(label):
    stamp = datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ')
    return RUNS_DIR / f'{stamp}-{label}'


def load_game_build(directory):
    """(executable bytes, description) of a tools/build_game.py output directory."""
    directory = Path(directory)
    build = load_json(directory / 'manifest.json')
    if build.get('format') != 'lezac-game-build/1':
        raise RunError(f'Not a game build directory (manifest.json): {directory}')
    exe = (directory / 'LEZAC.EXE').read_bytes()
    if sha256_bytes(exe) != build['outputs']['LEZAC.EXE']['sha256']:
        raise RunError('LEZAC.EXE does not match its build manifest')
    return exe, {'kind': 'rebuild', 'sha256': sha256_bytes(exe), 'bytes': len(exe),
                 'build_source': build['source'], 'build_toolchain': build['toolchain'],
                 'build_utc': build['built_utc']}


def prepare_run(run_dir=None, label='run', original_dir=ORIGINAL_DIR, allow_unpinned=False,
                identity=None, manifest=None, game_build=None):
    if not SAFE_LABEL.fullmatch(label):
        raise RunError('Labels use letters, digits, "-" and "_" (at most 64 characters)')
    run_dir = Path(run_dir) if run_dir else default_run_dir(label)
    run_dir = run_dir.absolute()
    if is_within(run_dir, Path(original_dir).parent) or is_within(Path(original_dir), run_dir):
        raise RunError('Run directories must be separate from the original analysis inputs')
    identity = identity if identity is not None else emulator_identity()
    if identity['mismatches'] and not allow_unpinned:
        raise RunError('Installed emulator is not the pinned reference: ' + '; '.join(identity['mismatches']) +
                       ' (pass --allow-unpinned-emulator to record an explicitly unpinned run)')
    originals = read_verified_originals(original_dir, manifest)
    files = dict(originals)
    game = {'kind': 'original', 'sha256': sha256_bytes(files['LEZAC.EXE']), 'bytes': len(files['LEZAC.EXE'])}
    if game_build is not None:
        files['LEZAC.EXE'], game = load_game_build(game_build)
    if run_dir.exists() and (run_dir.is_symlink() or not run_dir.is_dir() or any(run_dir.iterdir())):
        raise RunError(f'Refusing to reuse a non-empty run directory: {run_dir}')
    template = CONFIG_TEMPLATE.read_text()
    config = render_config(template, run_dir)
    run_dir.mkdir(parents=True, exist_ok=True, mode=0o700)
    for sub in ('C', 'capture', 'checkpoints', 'home'):
        (run_dir / sub).mkdir(mode=0o700)
    for name, data in files.items():
        if not SAFE_FILE.fullmatch(name):
            raise RunError(f'Unsafe original file name: {name!r}')
        with open(run_dir / 'C' / name, 'xb') as handle:
            handle.write(data)
    (run_dir / 'dosbox-x.conf').write_text(config)
    manifest = {
        'format': RUN_FORMAT,
        'created_utc': utc_now(),
        'run_dir': str(run_dir),
        'label': label,
        'source_revision': git_revision(),
        'tool_sha256': sha256_file(Path(__file__).resolve()),
        'originals': originals_summary(originals),
        'game_executable': game,
        'run_files': originals_summary(files),
        'config_template': {'path': str(CONFIG_TEMPLATE.relative_to(ROOT)),
                            'sha256': sha256_bytes(template.encode())},
        'config_sha256': sha256_bytes(config.encode()),
        'emulator': identity,
        'pinned_emulator': not identity['mismatches'],
        'sessions': [],
    }
    write_json(run_dir / 'run.json', manifest)
    return run_dir


def compare_run_copy(run_dir, originals):
    """Report how the game changed its private writable copy (e.g. score files)."""
    game_dir = Path(run_dir) / 'C'
    by_name = {item['name']: item for item in originals}
    changed, created, removed = [], [], []
    present = {entry.name: entry for entry in game_dir.iterdir()} if game_dir.is_dir() else {}
    for name, item in sorted(by_name.items()):
        entry = present.get(name)
        if entry is None:
            removed.append(name)
        elif entry.is_file() and sha256_file(entry) != item['sha256']:
            changed.append({'name': name, 'size': entry.stat().st_size, 'sha256': sha256_file(entry)})
    for name in sorted(set(present) - set(by_name)):
        entry = present[name]
        created.append({'name': name, 'size': entry.stat().st_size if entry.is_file() else None})
    return {'changed': changed, 'created': created, 'removed': removed}


# --------------------------------------------------------------------------
# Traces


def compile_trace(trace):
    """Validate a trace and return its absolute-time events."""
    if not isinstance(trace, dict) or trace.get('format') != TRACE_FORMAT:
        raise RunError(f'Trace format must be {TRACE_FORMAT!r}')
    if ('events' in trace) == ('steps' in trace):
        raise RunError('A trace has exactly one of "events" (absolute) or "steps" (relative)')
    if 'steps' in trace:
        events, now = [], 0
        for step in trace['steps']:
            if not isinstance(step, dict):
                raise RunError(f'Trace step is not an object: {step!r}')
            step = {key: value for key, value in step.items() if key != 'note'}
            if 'wait_ms' in step:
                if set(step) != {'wait_ms'}:
                    raise RunError('A wait step contains only wait_ms')
                now += _duration(step['wait_ms'])
            elif 'tap' in step:
                hold = _duration(step.get('hold_ms', 120))
                if set(step) - {'tap', 'hold_ms'}:
                    raise RunError('A tap step contains only tap and hold_ms')
                keys = step['tap'] if isinstance(step['tap'], list) else [step['tap']]
                events.extend({'t_ms': now, 'down': key} for key in keys)
                now += hold
                events.extend({'t_ms': now, 'up': key} for key in reversed(keys))
            else:
                events.append({'t_ms': now, **step})
        trace = {'format': TRACE_FORMAT, 'events': events}
    events = []
    held, last = set(), 0
    for raw in trace['events']:
        if not isinstance(raw, dict) or 't_ms' not in raw:
            raise RunError(f'Trace event lacks t_ms: {raw!r}')
        t_ms = raw['t_ms']
        if not isinstance(t_ms, (int, float)) or t_ms < last or t_ms > MAX_TRACE_MS:
            raise RunError(f'Trace times must be non-decreasing and bounded: {raw!r}')
        last = t_ms
        actions = set(raw) - {'t_ms', 'note'}
        if len(actions) != 1:
            raise RunError(f'Each trace event has exactly one action: {raw!r}')
        action = actions.pop()
        value = raw[action]
        if action in ('down', 'up'):
            if value not in GAME_KEYS:
                raise RunError(f'Key {value!r} is not an allowed game key')
            if action == 'down' and value in held:
                raise RunError(f'Key pressed twice without release: {value!r}')
            if action == 'up' and value not in held:
                raise RunError(f'Key released without a press: {value!r}')
            (held.add if action == 'down' else held.discard)(value)
        elif action == 'checkpoint':
            if not isinstance(value, str) or not SAFE_LABEL.fullmatch(value):
                raise RunError(f'Unsafe checkpoint name: {value!r}')
        elif action == 'video':
            if value not in ('start', 'stop'):
                raise RunError('video events are "start" or "stop"')
        elif action == 'end':
            if value is not True:
                raise RunError('end events use "end": true')
        else:
            raise RunError(f'Unknown trace action: {action!r}')
        events.append({'t_ms': t_ms, action: value})
    if held:
        raise RunError(f'Trace leaves keys pressed: {sorted(held)}')
    return events


def summarize_run(run_dir, trace=None):
    """Path-free summary of a finished run, suitable for committing as evidence."""
    manifest = load_json(Path(run_dir) / 'run.json')
    if not manifest['sessions']:
        raise RunError('The run has no finished session')
    session = manifest['sessions'][-1]
    identity = session['emulator_at_launch']
    summary = {
        'format': 'lezac-original-run-summary/1',
        'label': manifest['label'],
        'created_utc': manifest['created_utc'],
        'source_revision': manifest['source_revision'],
        'tool_sha256': manifest.get('tool_sha256'),
        'config_template': manifest['config_template'],
        'emulator': {key: identity.get(key) for key in ('version_banner', 'binary_sha256', 'packages')},
        'pinned_emulator': manifest['pinned_emulator'] and not identity['mismatches'],
        'originals': manifest['originals'],
        'game_executable': manifest.get('game_executable', {'kind': 'original'}),
        'session': {key: session.get(key) for key in (
            'mode', 'headless', 'started_utc', 'ended_utc', 'end_reason', 'emulator_returncode',
            'duration_game_ms', 'duration_wall_ms', 'event_count', 'originals_verified_before_launch',
            'originals_verified_after_exit', 'run_copy_changes')},
        'checkpoints': [{'name': c['name'], 'game_ms': c['game_ms'], 'frame_sha256': c['frame_sha256']}
                        for c in session['checkpoints']],
    }
    if 'trace' in session:
        summary['session']['replayed_trace_sha256'] = session['trace']['sha256']
    if trace is not None:
        summary['input_trace'] = {'path': str(Path(trace).resolve().relative_to(ROOT)),
                                  'sha256': sha256_file(trace)}
    return summary


def _duration(value):
    if not isinstance(value, (int, float)) or value < 0 or value > MAX_TRACE_MS:
        raise RunError(f'Invalid duration: {value!r}')
    return value


def export_trace(events, description):
    """Turn a session event log into a trace on the session's unpaused clock."""
    out = []
    for event in events:
        kind = event.get('event')
        # Releases issued before a pause stay: DOSBox-X releases held keys when it pauses.
        if kind in ('key_down', 'key_up'):
            out.append({'t_ms': round(event['game_ms'], 1), 'down' if kind == 'key_down' else 'up': event['key']})
        elif kind == 'checkpoint':
            out.append({'t_ms': round(event['game_ms'], 1), 'checkpoint': event['name']})
        elif kind == 'video':
            out.append({'t_ms': round(event['game_ms'], 1), 'video': event['state']})
    if events:
        out.append({'t_ms': round(events[-1]['game_ms'], 1), 'end': True})
    trace = {'format': TRACE_FORMAT, 'description': description, 'events': out}
    compile_trace(trace)
    return trace


# --------------------------------------------------------------------------
# X11 (Xvfb + XTest through ctypes; no extra Python dependency)


def write_xauthority(path, cookie):
    def field(data):
        return struct.pack('>H', len(data)) + data
    # FamilyWild with empty address and display number matches any local display.
    entry = struct.pack('>H', 0xFFFF) + field(b'') + field(b'') + field(b'MIT-MAGIC-COOKIE-1') + field(cookie)
    fd = os.open(path, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600)
    with os.fdopen(fd, 'wb') as handle:
        handle.write(entry)


class Xvfb:
    def __init__(self, workdir):
        self.workdir = Path(workdir)
        self.authority = self.workdir / 'xauthority'
        self.process = None
        self.display = None

    def start(self):
        binary = shutil.which('Xvfb')
        if not binary:
            raise RunError('Xvfb is required for headless runs')
        write_xauthority(self.authority, secrets.token_bytes(16))
        read_fd, write_fd = os.pipe()
        log = open(self.workdir / 'xvfb.log', 'ab')
        try:
            self.process = subprocess.Popen(
                [binary, '-displayfd', str(write_fd), '-auth', str(self.authority), '-nolisten', 'tcp',
                 '-screen', '0', '1024x768x24', '-noreset'],
                pass_fds=(write_fd,), stdout=log, stderr=log, start_new_session=True)
        finally:
            os.close(write_fd)
            log.close()
        with os.fdopen(read_fd) as pipe:
            number = pipe.readline().strip()
        if not number.isdigit():
            self.stop()
            raise RunError('Xvfb did not report a display number')
        self.display = f':{number}'
        return self.display

    def stop(self):
        if self.process and self.process.poll() is None:
            self.process.terminate()
            try:
                self.process.wait(10)
            except subprocess.TimeoutExpired:
                self.process.kill()
                self.process.wait()


class XDisplay:
    REVERT_TO_PARENT = 2

    def __init__(self, name, authority=None):
        if authority:
            os.environ['XAUTHORITY'] = str(authority)
        c = ctypes
        self.x11 = c.CDLL(c.util.find_library('X11') or 'libX11.so.6')
        self.xtst = c.CDLL(c.util.find_library('Xtst') or 'libXtst.so.6')
        x = self.x11
        x.XOpenDisplay.restype = c.c_void_p
        x.XOpenDisplay.argtypes = [c.c_char_p]
        x.XDefaultRootWindow.restype = c.c_ulong
        x.XDefaultRootWindow.argtypes = [c.c_void_p]
        x.XQueryTree.argtypes = [c.c_void_p, c.c_ulong, c.POINTER(c.c_ulong), c.POINTER(c.c_ulong),
                                 c.POINTER(c.POINTER(c.c_ulong)), c.POINTER(c.c_uint)]
        x.XFetchName.argtypes = [c.c_void_p, c.c_ulong, c.POINTER(c.c_void_p)]
        x.XFree.argtypes = [c.c_void_p]
        x.XStringToKeysym.restype = c.c_ulong
        x.XStringToKeysym.argtypes = [c.c_char_p]
        x.XKeysymToKeycode.restype = c.c_ubyte
        x.XKeysymToKeycode.argtypes = [c.c_void_p, c.c_ulong]
        x.XSetInputFocus.argtypes = [c.c_void_p, c.c_ulong, c.c_int, c.c_ulong]
        x.XGetInputFocus.argtypes = [c.c_void_p, c.POINTER(c.c_ulong), c.POINTER(c.c_int)]
        x.XSync.argtypes = [c.c_void_p, c.c_int]
        x.XCloseDisplay.argtypes = [c.c_void_p]
        self.xtst.XTestFakeKeyEvent.argtypes = [c.c_void_p, c.c_uint, c.c_int, c.c_ulong]
        handler_type = c.CFUNCTYPE(c.c_int, c.c_void_p, c.c_void_p)
        self.errors = 0

        def on_error(_display, _event):
            self.errors += 1
            return 0
        self._handler = handler_type(on_error)  # keep a reference for the C library
        x.XSetErrorHandler(self._handler)
        self.dpy = x.XOpenDisplay(name.encode())
        if not self.dpy:
            raise RunError(f'Cannot open X display {name}')
        self.root = x.XDefaultRootWindow(self.dpy)
        self.keycodes = {}

    def windows(self):
        c = ctypes
        root, parent = c.c_ulong(), c.c_ulong()
        children, count = c.POINTER(c.c_ulong)(), c.c_uint()
        if not self.x11.XQueryTree(self.dpy, self.root, c.byref(root), c.byref(parent),
                                   c.byref(children), c.byref(count)):
            return []
        found = []
        for index in range(count.value):
            window = children[index]
            name = c.c_void_p()
            if self.x11.XFetchName(self.dpy, window, c.byref(name)) and name.value:
                found.append((window, c.cast(name, c.c_char_p).value.decode('latin1')))
                self.x11.XFree(name)
        if children:
            self.x11.XFree(children)
        return found

    def focus(self, window):
        self.x11.XSetInputFocus(self.dpy, window, self.REVERT_TO_PARENT, 0)
        self.x11.XSync(self.dpy, 0)
        focused, revert = ctypes.c_ulong(), ctypes.c_int()
        self.x11.XGetInputFocus(self.dpy, ctypes.byref(focused), ctypes.byref(revert))
        return focused.value == window

    def keycode(self, name):
        if name not in self.keycodes:
            keysym = self.x11.XStringToKeysym(name.encode())
            code = self.x11.XKeysymToKeycode(self.dpy, keysym) if keysym else 0
            if not code:
                raise RunError(f'No X keycode for key {name!r}')
            self.keycodes[name] = code
        return self.keycodes[name]

    def key(self, name, pressed):
        self.xtst.XTestFakeKeyEvent(self.dpy, self.keycode(name), 1 if pressed else 0, 0)
        self.x11.XSync(self.dpy, 0)

    def close(self):
        if self.dpy:
            self.x11.XCloseDisplay(self.dpy)
            self.dpy = None


# --------------------------------------------------------------------------
# Emulator sessions


def sleep_until(deadline):
    while True:
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            return
        time.sleep(remaining - 0.001 if remaining > 0.002 else 0)


def frame_digest(path):
    """SHA-256 over dimensions, palette and indices of a raw DOSBox-X capture."""
    try:
        from PIL import Image
    except ImportError:
        return None
    with Image.open(path) as image:
        if image.mode != 'P':
            return None
        palette = bytes(image.getpalette()[:768])
        header = b'LZFRAME1' + struct.pack('<HH', *image.size)
        return sha256_bytes(header + palette + image.tobytes())


class EmulatorSession:
    """One emulator process for one run directory, with a timestamped event log."""

    def __init__(self, run_dir, headless=True, time_limit=3600, mode='play'):
        self.run_dir = Path(run_dir).absolute()
        self.headless = headless
        self.time_limit = time_limit
        self.mode = mode
        self.manifest = load_json(self.run_dir / 'run.json')
        if self.manifest.get('format') != RUN_FORMAT:
            raise RunError('Not a prepared run directory (missing run.json)')
        if self.manifest['sessions']:
            raise RunError('This run directory was already used; prepare a fresh one')
        self.xvfb = None
        self.x = None
        self.display = None
        self.process = None
        self.window = None
        self.origin = None
        self.paused_since = None
        self.paused_total = 0.0
        self.held = []
        self.video = False
        self.sequence = 0
        self.checkpoints = []
        self.events_path = self.run_dir / 'events.jsonl'
        self.events = []

    # -- clock and log
    def wall_ms(self):
        return (time.monotonic() - self.origin) * 1000.0

    def game_ms(self, now=None):
        now = time.monotonic() if now is None else now
        paused = self.paused_total + ((now - self.paused_since) if self.paused_since is not None else 0.0)
        return (now - self.origin - paused) * 1000.0

    def log(self, kind, **fields):
        now = time.monotonic()
        event = {'seq': self.sequence, 'event': kind,
                 'wall_ms': round((now - self.origin) * 1000.0, 3) if self.origin else None,
                 'game_ms': round(self.game_ms(now), 3) if self.origin else None, **fields}
        self.sequence += 1
        self.events.append(event)
        with open(self.events_path, 'a') as handle:
            handle.write(json.dumps(event, sort_keys=True) + '\n')
        return event

    # -- lifecycle
    def start(self):
        identity = emulator_identity()
        if identity['mismatches'] and self.manifest['pinned_emulator']:
            raise RunError('Emulator changed since prepare: ' + '; '.join(identity['mismatches']))
        before = read_verified_originals()
        self.session = {'mode': self.mode, 'headless': self.headless, 'started_utc': utc_now(),
                        'emulator_at_launch': identity,
                        'originals_verified_before_launch': len(before) == len(self.manifest['originals'])}
        env = {'PATH': os.environ.get('PATH', '/usr/bin:/bin'), 'HOME': str(self.run_dir / 'home'),
               'XDG_CONFIG_HOME': str(self.run_dir / 'home' / '.config'), 'LANG': 'C.UTF-8'}
        if self.headless:
            self.xvfb = Xvfb(self.run_dir / 'home')
            self.display = self.xvfb.start()
            env['XAUTHORITY'] = str(self.xvfb.authority)
            env['SDL_AUDIODRIVER'] = 'dummy'
        else:
            self.display = os.environ.get('DISPLAY')
            if not self.display:
                raise RunError('Interactive launch needs DISPLAY (use play/session-start for headless runs)')
            for name in ('XAUTHORITY', 'PULSE_SERVER', 'XDG_RUNTIME_DIR', 'WAYLAND_DISPLAY'):
                if name in os.environ:
                    env[name] = os.environ[name]
        env['DISPLAY'] = self.display
        binary = identity['resolved_binary']
        command = [binary, '-conf', str(self.run_dir / 'dosbox-x.conf'), '-defaultmapper', '-fastlaunch']
        if self.time_limit:
            command += ['-time-limit', str(int(self.time_limit))]
        self.session['command'] = command
        self.stdout = open(self.run_dir / 'dosbox-x.stdout', 'ab')
        self.process = subprocess.Popen(command, cwd=self.run_dir, env=env, stdout=self.stdout,
                                        stderr=subprocess.STDOUT, start_new_session=True)
        if not self.headless:
            # A person drives the window; a desktop window manager owns its focus.
            self.origin = time.monotonic()
            self.log('launched', display=self.display)
            return
        self.x = XDisplay(self.display, self.xvfb.authority)
        deadline = time.monotonic() + 30
        while time.monotonic() < deadline:
            self.ensure_running()
            windows = self.x.windows()
            ours = [w for w, name in windows if name.startswith('DOSBox-X')]
            if ours and self.x.focus(ours[0]):
                self.window = ours[0]
                break
            time.sleep(0.02)
        else:
            raise RunError('The DOSBox-X window did not appear')
        self.origin = time.monotonic()
        self.log('window', display=self.display, window=self.window)

    def ensure_running(self):
        if self.process.poll() is not None:
            raise RunError(f'DOSBox-X exited with status {self.process.returncode}')
        if self.headless and self.window is not None:
            others = [name for _w, name in self.x.windows() if not name.startswith('DOSBox-X')]
            if others:
                raise RunError(f'Unexpected window (likely a blocking emulator dialog): {others}')

    def finish(self, reason):
        if self.process is None:
            if self.xvfb:
                self.xvfb.stop()
            return
        try:
            if self.process.poll() is None:
                self.log('quit', reason=reason)
                try:
                    if self.x is None:
                        raise RunError('no X connection for a graceful quit')
                    self.resume()  # a paused DOSBox-X ignores every hotkey except unpause
                    if self.video:
                        self.toggle_video('stop')
                        time.sleep(1.0)
                    self.release_all('finish')
                    self.hotkey('Control_L', 'F9')
                    self.process.wait(15)
                except (RunError, OSError, subprocess.TimeoutExpired) as failure:
                    self.log('forced_stop', reason=str(failure))
                    self.process.terminate()
                    try:
                        self.process.wait(10)
                    except subprocess.TimeoutExpired:
                        self.process.kill()
                        self.process.wait()
            else:
                self.log('emulator_exited', returncode=self.process.returncode)
        finally:
            if self.x:
                self.x.close()
            if self.xvfb:
                self.xvfb.stop()
            self.stdout.close()
            self.session.update({
                'ended_utc': utc_now(),
                'end_reason': reason,
                'emulator_returncode': self.process.returncode,
                'events_file': 'events.jsonl',
                'event_count': self.sequence,
                'checkpoints': self.checkpoints,
                'duration_wall_ms': round(self.wall_ms(), 3) if self.origin else None,
                'duration_game_ms': round(self.game_ms(), 3) if self.origin else None,
                'run_copy_changes': compare_run_copy(self.run_dir, self.manifest.get('run_files',
                                                                                     self.manifest['originals'])),
            })
            try:
                read_verified_originals()
                self.session['originals_verified_after_exit'] = True
            except RunError as error:
                self.session['originals_verified_after_exit'] = False
                self.session['originals_error'] = str(error)
            self.manifest['sessions'].append(self.session)
            write_json(self.run_dir / 'run.json', self.manifest)
            self.process = None

    # -- input
    def key_down(self, key, source='input'):
        if key not in GAME_KEYS:
            raise RunError(f'Key {key!r} is not an allowed game key')
        if self.paused_since is not None:
            raise RunError('Emulation is paused; DOSBox-X discards keys while paused')
        if key in self.held:
            raise RunError(f'Key already held: {key}')
        self.ensure_running()
        self.x.key(key, True)
        self.held.append(key)
        self.log('key_down', key=key, source=source)

    def key_up(self, key, source='input'):
        if key not in self.held:
            raise RunError(f'Key is not held: {key}')
        self.x.key(key, False)
        self.held.remove(key)
        self.log('key_up', key=key, source=source)

    def release_all(self, source):
        for key in list(reversed(self.held)):
            self.key_up(key, source=source)

    def hotkey(self, *keys):
        for key in keys:
            self.x.key(key, True)
        for key in reversed(keys):
            self.x.key(key, False)

    def pause(self):
        if self.paused_since is not None:
            return
        self.release_all('pause')
        self.ensure_running()
        self.hotkey(HOST_KEY, 'Pause')
        self.paused_since = time.monotonic()
        self.log('pause')

    def resume(self):
        if self.paused_since is None:
            return
        self.ensure_running()
        self.log('resume')
        self.hotkey(HOST_KEY, 'Pause')
        self.paused_total += time.monotonic() - self.paused_since
        self.paused_since = None

    def toggle_video(self, state):
        if state == ('start' if self.video else 'stop'):
            return
        if self.paused_since is not None:
            raise RunError('Video capture toggles need running emulation; DOSBox-X ignores hotkeys while paused')
        self.ensure_running()
        self.hotkey(HOST_KEY, 'i')
        self.video = state == 'start'
        self.log('video', state=state)

    def checkpoint(self, name):
        if not SAFE_LABEL.fullmatch(name):
            raise RunError(f'Unsafe checkpoint name: {name!r}')
        if self.paused_since is not None:
            raise RunError('Checkpoints need running emulation: DOSBox-X renders no frames while paused')
        self.ensure_running()
        capture = self.run_dir / 'capture'
        before = {path.name for path in capture.glob('*.png')}
        event = self.log('checkpoint', name=name)
        self.hotkey(HOST_KEY, 'Control_L', 'p')
        deadline = time.monotonic() + 5
        new = []
        while time.monotonic() < deadline and not new:
            time.sleep(0.01)
            new = sorted(path for path in capture.glob('*.png') if path.name not in before)
        if not new:
            raise RunError(f'No raw screenshot appeared for checkpoint {name}')
        size = -1
        while time.monotonic() < deadline:  # DOSBox-X may still be writing the PNG
            time.sleep(0.05)
            current = new[-1].stat().st_size
            if current == size and current > 0:
                break
            size = current
        target = self.run_dir / 'checkpoints' / f'{len(self.checkpoints):03d}-{name}.png'
        os.replace(new[-1], target)
        record = {'name': name, 'seq': event['seq'], 'wall_ms': event['wall_ms'], 'game_ms': event['game_ms'],
                  'file': str(target.relative_to(self.run_dir)), 'sha256': sha256_file(target),
                  'frame_sha256': frame_digest(target)}
        self.checkpoints.append(record)
        return record

    # -- trace replay
    def play(self, events):
        for event in events:
            sleep_until(self.origin + event['t_ms'] / 1000.0)
            if 'down' in event:
                self.key_down(event['down'], source='trace')
            elif 'up' in event:
                self.key_up(event['up'], source='trace')
            elif 'checkpoint' in event:
                self.checkpoint(event['checkpoint'])
            elif 'video' in event:
                self.toggle_video(event['video'])
            elif 'end' in event:
                break

    # -- session commands
    def command(self, line):
        parts = line.split()
        if not parts:
            raise RunError('Empty command')
        verb, args = parts[0], parts[1:]

        def arity(count):
            if len(args) != count:
                raise RunError(f'{verb} takes {count} argument(s)')
        if verb == 'down':
            arity(1)
            self.key_down(args[0])
        elif verb == 'up':
            arity(1)
            self.key_up(args[0])
        elif verb == 'tap':
            if len(args) not in (1, 2):
                raise RunError('tap KEY[+KEY...] [HOLD_MS]')
            keys = args[0].split('+')
            hold = float(args[1]) if len(args) == 2 else 120.0
            for key in keys:
                self.key_down(key)
            sleep_until(time.monotonic() + _duration(hold) / 1000.0)
            for key in reversed(keys):
                self.key_up(key)
        elif verb == 'wait':
            arity(1)
            sleep_until(time.monotonic() + _duration(float(args[0])) / 1000.0)
        elif verb == 'pause':
            arity(0)
            self.pause()
        elif verb == 'resume':
            arity(0)
            self.resume()
        elif verb == 'checkpoint':
            arity(1)
            return self.checkpoint(args[0])
        elif verb == 'video':
            arity(1)
            if args[0] not in ('start', 'stop'):
                raise RunError('video start|stop')
            self.toggle_video(args[0])
        elif verb == 'note':
            self.log('note', text=' '.join(args))
        elif verb == 'status':
            arity(0)
        else:
            raise RunError(f'Unknown command: {verb}')
        return {'game_ms': round(self.game_ms(), 1), 'paused': self.paused_since is not None,
                'held': list(self.held)}


# --------------------------------------------------------------------------
# Session server (closed-loop exploration and recording)


def session_socket_path(run_dir):
    return Path(run_dir).absolute() / 'session.sock'


def serve_session(run_dir, time_limit):
    session = EmulatorSession(run_dir, headless=True, time_limit=time_limit, mode='session')
    path = session_socket_path(run_dir)
    server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    server.bind(str(path))
    os.chmod(path, 0o600)
    server.listen(1)
    server.settimeout(1.0)
    reason = 'stop command'
    try:
        session.start()
        (Path(run_dir) / 'session.ready').write_text('ready\n')
        running = True
        while running:
            try:
                connection, _ = server.accept()
            except socket.timeout:
                if session.process.poll() is not None:
                    reason = 'emulator exited'
                    break
                continue
            with connection:
                results, error = [], None
                try:
                    commands = json.loads(connection.makefile('r').readline() or '{}').get('commands', [])
                    if not isinstance(commands, list) or not all(isinstance(c, str) for c in commands):
                        raise ValueError('commands must be a list of strings')
                except (ValueError, AttributeError) as failure:
                    commands, error = [], f'bad request: {failure}'
                for line in commands:
                    if line.strip() == 'stop':
                        running = False
                        break
                    try:
                        results.append(session.command(line))
                    except (RunError, ValueError) as failure:
                        error = f'{line}: {failure}'
                        break
                connection.sendall((json.dumps({'ok': error is None, 'error': error,
                                                'results': results}) + '\n').encode())
    except BaseException as failure:  # record the reason, then always finalize the run
        reason = f'error: {failure}'
        raise
    finally:
        try:
            session.finish(reason)
        finally:
            server.close()
            path.unlink(missing_ok=True)
            (Path(run_dir) / 'session.ready').unlink(missing_ok=True)


def send_session(run_dir, commands, timeout=600):
    client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    client.settimeout(timeout)
    client.connect(str(session_socket_path(run_dir)))
    with client:
        client.sendall((json.dumps({'commands': commands}) + '\n').encode())
        return json.loads(client.makefile('r').readline())


# --------------------------------------------------------------------------
# CLI


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest='command', required=True)
    sub.add_parser('verify')
    sub.add_parser('check-emulator')
    prepare = sub.add_parser('prepare')
    prepare.add_argument('--run-dir', type=Path)
    prepare.add_argument('--label', default='run')
    prepare.add_argument('--allow-unpinned-emulator', action='store_true')
    prepare.add_argument('--game-build', type=Path,
                         help='run the source-built LEZAC.EXE from this tools/build_game.py output')
    launch = sub.add_parser('launch')
    launch.add_argument('run_dir', type=Path)
    launch.add_argument('--time-limit', type=int, default=0)
    play = sub.add_parser('play')
    play.add_argument('run_dir', type=Path)
    play.add_argument('trace', type=Path)
    play.add_argument('--settle-ms', type=int, default=2000,
                      help='keep running after the last event before quitting')
    start = sub.add_parser('session-start')
    start.add_argument('run_dir', type=Path)
    start.add_argument('--time-limit', type=int, default=3600)
    serve = sub.add_parser('session-serve')
    serve.add_argument('run_dir', type=Path)
    serve.add_argument('--time-limit', type=int, default=3600)
    send = sub.add_parser('session-send')
    send.add_argument('run_dir', type=Path)
    send.add_argument('commands', nargs='+')
    export = sub.add_parser('export-trace')
    export.add_argument('run_dir', type=Path)
    export.add_argument('output', type=Path)
    export.add_argument('--description', default='Exported from a recorded original-game session')
    summary = sub.add_parser('summarize')
    summary.add_argument('run_dir', type=Path)
    summary.add_argument('output', type=Path)
    summary.add_argument('--trace', type=Path, help='committed input trace that belongs to this run')
    args = parser.parse_args(argv)

    try:
        if args.command == 'verify':
            files = read_verified_originals()
            print(json.dumps({'verified': len(files), 'files': originals_summary(files)}, indent=2))
        elif args.command == 'check-emulator':
            identity = emulator_identity()
            print(json.dumps(identity, indent=2))
            return 1 if identity['mismatches'] else 0
        elif args.command == 'prepare':
            print(prepare_run(args.run_dir, args.label, allow_unpinned=args.allow_unpinned_emulator,
                              game_build=args.game_build))
        elif args.command == 'launch':
            session = EmulatorSession(args.run_dir, headless=False, time_limit=args.time_limit, mode='interactive')
            session.start()
            try:
                session.process.wait()
            finally:
                session.finish('interactive session ended')
            return report(args.run_dir)
        elif args.command == 'play':
            trace = load_json(args.trace)
            events = compile_trace(trace)
            last = events[-1]['t_ms'] if events else 0
            session = EmulatorSession(args.run_dir, headless=True,
                                      time_limit=int(last / 1000 + args.settle_ms / 1000 + 60), mode='play')
            reason = 'trace complete'
            try:
                session.start()
                session.session['trace'] = {'path': str(args.trace), 'sha256': sha256_file(args.trace),
                                            'events': len(events)}
                session.play(events)
                sleep_until(time.monotonic() + args.settle_ms / 1000.0)
            except BaseException as failure:
                reason = f'error: {failure}'
                raise
            finally:
                session.finish(reason)
            return report(args.run_dir)
        elif args.command == 'session-start':
            log = open(Path(args.run_dir) / 'session.log', 'ab')
            subprocess.Popen([sys.executable, str(Path(__file__).resolve()), 'session-serve', str(args.run_dir),
                              '--time-limit', str(args.time_limit)],
                             stdin=subprocess.DEVNULL, stdout=log, stderr=log, start_new_session=True)
            deadline = time.monotonic() + 45
            while not (Path(args.run_dir) / 'session.ready').exists():
                if time.monotonic() > deadline:
                    raise RunError('Session did not become ready; see session.log')
                time.sleep(0.1)
            print(f'session ready: {Path(args.run_dir).absolute()}')
        elif args.command == 'session-serve':
            serve_session(args.run_dir, args.time_limit)
        elif args.command == 'session-send':
            reply = send_session(args.run_dir, args.commands)
            print(json.dumps(reply, indent=2))
            return 0 if reply.get('ok') else 1
        elif args.command == 'export-trace':
            events = [json.loads(line) for line in (Path(args.run_dir) / 'events.jsonl').read_text().splitlines()]
            write_json(args.output, export_trace(events, args.description))
            print(args.output)
        elif args.command == 'summarize':
            write_json(args.output, summarize_run(args.run_dir, args.trace))
            print(args.output)
    except RunError as error:
        print(f'error: {error}', file=sys.stderr)
        return 2
    return 0


def report(run_dir):
    manifest = load_json(Path(run_dir) / 'run.json')
    session = manifest['sessions'][-1]
    summary = {key: session.get(key) for key in (
        'mode', 'end_reason', 'emulator_returncode', 'originals_verified_after_exit', 'run_copy_changes')}
    summary['checkpoints'] = [(c['name'], c['frame_sha256']) for c in session.get('checkpoints', [])]
    print(json.dumps(summary, indent=2))
    return 0 if session.get('originals_verified_after_exit') else 3


if __name__ == '__main__':
    sys.exit(main())
