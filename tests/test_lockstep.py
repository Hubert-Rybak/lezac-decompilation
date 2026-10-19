"""Gameplay comparison: the source-built game and the original run in lockstep.

Each input trace drives the authenticated original LEZAC.EXE and the fresh
build of pascal/LEZAC.PAS in two deterministic machines (tools/lockstep.py).
At every Delay call / input wait the screens, palettes, PC-speaker writes,
all named game variables, the heap and the files the game writes must be
identical. Scenario checks then confirm the trace really exercised what it is
meant to (level start, death and restart, level completion and transition,
two players, menus, levels 2-7, high-score files).

These runs take minutes; they execute when LEZAC_GAMEPLAY_TESTS=1 (the
`gameplay-lockstep` CI job and `make lockstep`). Reports are written to
build/lockstep/ (hashes and variable values only, no images).
"""
import hashlib
import json
import os
import struct
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

pytestmark = pytest.mark.skipif(os.environ.get('LEZAC_GAMEPLAY_TESTS') != '1',
                                reason='gameplay lockstep runs only with LEZAC_GAMEPLAY_TESTS=1')

REPORTS = ROOT / 'build' / 'lockstep'


@pytest.fixture(scope='module')
def game_build(tmp_path_factory):
    if not (ROOT / 'original/game/LEZAC.EXE').is_file():
        pytest.skip('original/game not imported (tools/fetch_original.py)')
    import build_game
    try:
        build_game.find_compiler()
    except build_game.BuildError as error:
        pytest.skip(str(error))
    output = tmp_path_factory.mktemp('lockstep') / 'game'
    build_game.build(output)
    return output


def run(game_build, trace_path, max_steps):
    import lockstep
    trace = json.loads(trace_path.read_text())
    report = lockstep.run_lockstep(game_build, trace, max_steps, quiet=True)
    REPORTS.mkdir(parents=True, exist_ok=True)
    (REPORTS / f'{trace_path.stem}.json').write_text(json.dumps(report, indent=2) + '\n')
    assert report['divergence'] is None, json.dumps(report['divergence'], indent=2)
    return report


def changes(report, name):
    return [(m['step'], m['changed'][name]) for m in report['milestones'] if name in m['changed']]


def test_boot_menu_level_start_death_and_continue(game_build):
    report = run(game_build, ROOT / 'emulator/traces/boot-smoke.json', 4000)
    assert changes(report, 'Level')[0][1] == [0, 1]
    # a new game sets 2 spare lives; one death in the trace costs one of them
    assert [changed for _step, changed in changes(report, 'Lives[1]')] == [[0, 2], [2, 1]]
    assert [changed for _step, changed in changes(report, 'PlayerState[1]')] == [[0, 1], [1, 2], [2, 1]]
    assert report['final_event'] == ['stalled']


def test_recorded_reference_session(game_build):
    report = run(game_build, ROOT / 'emulator/traces/reference-level1.json', 20000)
    assert report['final_event'] == ['exit', 0]
    assert report['console_original'] == report['console_rebuild']


def test_exploration_session(game_build):
    run(game_build, ROOT / 'emulator/traces/exploration-level1.json', 20000)


def test_two_player_session(game_build):
    report = run(game_build, ROOT / 'emulator/traces/two-player-controls.json', 20000)
    assert any(changed == [0, 2] for _step, changed in changes(report, 'Players'))


def test_level_one_completed_and_level_two_started(game_build):
    report = run(game_build, ROOT / 'emulator/lockstep/level1-complete.json', 20000)
    levels = [changed for _step, changed in changes(report, 'Level')]
    assert [1, 2] in levels
    assert any(changed == [0, 1] for _step, changed in changes(report, 'BonusDone'))
    assert any(changed == [0, 1] for _step, changed in changes(report, 'DestructionDone'))


@pytest.mark.parametrize('level', [2, 3, 4, 5, 6, 7])
def test_levels_two_to_seven(game_build, level):
    """Entered through the trace option start_level (Level set at the first
    StartLevel entry in both machines), played with scripted input, then the
    game is abandoned and the program left."""
    report = run(game_build, ROOT / f'emulator/lockstep/level{level}-play.json', 20000)
    assert changes(report, 'Level')[0][1] == [0, level]
    assert report['final_event'] == ['exit', 0]


def records(entries):
    """RECS.DAT as the game writes it: a count byte, then score + string[8]."""
    data = bytearray([7])
    for score, name in entries:
        data += struct.pack('<i', score) + bytes([8]) + name.ljust(8).encode()
    return hashlib.sha256(bytes(data)).hexdigest()


def test_high_score_entry_is_saved(game_build):
    report = run(game_build, ROOT / 'emulator/lockstep/high-score-entry.json', 20000)
    assert report['files_written'] == {'RECS.DAT': records(
        [(20000, 'uno'), (15000, 'due'), (10000, 'tre'), (8000, 'quattro'), (6450, 'inerxzce'),
         (5000, 'cinque'), (3000, 'sei')])}


def test_tie_with_the_seventh_score_writes_past_the_table(game_build):
    report = run(game_build, ROOT / 'emulator/lockstep/high-score-tie.json', 20000)
    assert report['final_event'] == ['stop_ms', 30000]
    # the new entry went to HighScores[8]: the saved table is unchanged
    assert report['files_written'] == {'RECS.DAT': records(
        [(20000, 'uno'), (15000, 'due'), (10000, 'tre'), (8000, 'quattro'), (5000, 'cinque'),
         (3000, 'sei'), (0, 'sette')])}


def test_missing_records_file_is_created(game_build):
    report = run(game_build, ROOT / 'emulator/lockstep/records-missing.json', 20000)
    assert report['files_written'] == {'RECS.DAT': records([(10000, 'nessuno')] * 7)}
