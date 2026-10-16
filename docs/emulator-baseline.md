# Original-game emulator baseline

This is the reference setup for running the **authentic original** `LEZAC.EXE`
(issue #3). It pins a full DOS emulator and its configuration, runs the game only
from disposable, hash-verified copies, and records input traces and exact
framebuffer checkpoints. It is evidence about the original game. It does not
rebuild or run reconstructed code, and it is **not a deterministic gameplay replay
system**. Exact replay needs emulated-time input injection, which the pinned
emulator does not have (see [Nondeterminism](#sources-of-nondeterminism-measured)).

Captures, videos, run directories and original files stay local under ignored
`build/` and `original/`. The repository holds the tool, the configuration,
input traces written by us, and path-free run summaries with SHA-256 frame
digests. No images or game bytes are committed.

## Pinned emulator

| Item | Pinned value |
|---|---|
| Emulator | DOSBox-X **2024.03.01** (SDL2) |
| Distribution | Ubuntu 24.04 (noble) universe: `dosbox-x` and `dosbox-x-data` `2024.03.01+dfsg-1build2` |
| `dosbox-x` .deb SHA-256 | `1a27eb70d26360a0b82145ab4d1035e2e02d8d844211985758d51f6c854f874b` |
| `dosbox-x-data` .deb SHA-256 | `48f393f48415d246236e4fa4288d0ef234e365bcf4656f38692f7a1add76867f` |
| `/usr/bin/dosbox-x` SHA-256 | `30a5fdf8fa95abaf7bae1a9e624ccfc9e26e5357a19cc567bf3a2ac659699258` |
| Upstream source | `dosbox-x_2024.03.01+dfsg.orig.tar.xz`, SHA-256 `b03cdf5fc2806245cf4ec62f722b645478da31c99df156ba97f7c6661f9379e4` |
| Configuration | [`emulator/dosbox-x-reference.conf`](../emulator/dosbox-x-reference.conf), SHA-256 in [`analysis/emulator.json`](../analysis/emulator.json) |

```sh
sudo apt-get install dosbox-x=2024.03.01+dfsg-1build2 dosbox-x-data=2024.03.01+dfsg-1build2 xvfb
python3 tools/run_original.py check-emulator   # exits nonzero on any mismatch
```

`prepare` refuses an emulator whose binary hash, version banner or package
versions differ from the pin. `--allow-unpinned-emulator` lets you try another
host, and the run is then recorded as `"pinned_emulator": false`. The SDL2 and
Xvfb package versions are recorded but not pinned.

### Configuration choices

Only the settings below are set. Every other setting is a built-in default of the
pinned binary. With `-conf`, DOSBox-X loads no user or system configuration file
(`src/gui/sdlmain.cpp`), and `-defaultmapper` ignores any user key-mapping file.

| Setting | Value | Why |
|---|---|---|
| `machine` | `vgaonly` | The game requires plain VGA (`ENGLISH.DOC`). There are no SVGA extensions to depend on. |
| `cputype`, `core` | `486`, `normal` | "386 or better, 486 recommended". The interpreter core keeps execution simple to reason about. |
| `cycles` | `fixed 26800` | Roughly a 486DX2-66. The game times 30-frame blocks with DOS GetTime (`0x1247f`, main loop at `0x17919`) and adapts its speed. A fixed rate makes that adaptation repeatable. Other rates are untested. |
| `memsize` | `4` | About 300 KB of conventional memory is needed; 4 MB is a plausible 1996 machine. |
| `pcspeaker` / `sbtype` / `gus` / `mpu401` | on / none / off / none | The game drives the PC speaker. Sound cards and MIDI are disabled so their presence cannot change behavior. |
| `joysticktype` | `none` | The game reads only the keyboard. |
| `keyboardlayout` | `us` | Matches the documented key names. `auto` logged a missing-layout error. |
| `synchronize time` | `false` | Together with `date 04-23-1996` / `time 12:00:00` in AUTOEXEC, the DOS clock starts from a fixed value instead of host time. |
| `saveremark`, `show recorded filename` | `false` | Both would open blocking dialogs in a headless run. |
| AUTOEXEC | `mount c "<run>/C"`, `LEZAC.EXE`, `exit` | Only the run copy is mounted. Leaving the game closes the emulator. |

Headless runs use a private Xvfb (`1024x768x24`) that does not listen on TCP and
is cookie-authenticated (a fresh `MIT-MAGIC-COOKIE-1` per run). They also set
`SDL_AUDIODRIVER=dummy`, so the mixer still runs and video captures include audio.
`HOME` points into the run directory. Interactive launches use your `DISPLAY` and
audio.

## Running the original

```sh
python3 tools/fetch_original.py                     # authenticated import (once)
python3 tools/run_original.py verify                # all 14 files, no extra entries
RUN=$(python3 tools/run_original.py prepare --label mine)

python3 tools/run_original.py launch "$RUN"         # interactive, on your display
python3 tools/run_original.py play "$RUN" emulator/traces/boot-smoke.json   # automated, headless
```

Each run directory is used once. It contains:

- `C/`: writable copies of the 14 verified files, which is the only mounted drive.
- `dosbox-x.conf`: the rendered configuration.
- `run.json`: input hashes, emulator identity, the config and tool hashes, the git
  revision, and any uncommitted files that define the run.
- `events.jsonl`: every key, pause and checkpoint, with host time and unpaused time.
- `checkpoints/`: renamed raw captures.
- `capture/`: AVI and other captures.
- Logs.

The tool authenticates `original/game` before launch and again after exit. The
check fails on any changed, missing or extra file, so a launch inside the analysis
directory would be detected. The tool never mounts that directory. After exit it
also records which files the game changed in its own copy, for example `RECS.DAT`
after a high score.

### Closed-loop sessions and traces

Driving the game by hand, or as an agent looking at screenshots, uses a session:

```sh
python3 tools/run_original.py session-start "$RUN"
python3 tools/run_original.py session-send "$RUN" "resume" "tap x 400" "checkpoint after-move" "pause"
python3 tools/run_original.py session-send "$RUN" stop
python3 tools/run_original.py export-trace "$RUN" my-trace.json
python3 tools/run_original.py summarize "$RUN" summary.json --trace my-trace.json
```

Commands are `down`/`up`/`tap KEY[+KEY] [ms]`, `wait ms`, `pause`/`resume`
(DOSBox-X F12+Pause), `checkpoint NAME`, `video start|stop` and `note`. Only the
game's own keys are accepted. Host hotkeys come only from dedicated commands, and
the tool never sends the save-state hotkey. Pausing first releases held keys,
because DOSBox-X releases them and drops input while paused. `export-trace`
rebuilds the inputs on the **unpaused clock**. Traces are JSON (`lezac-input-trace/1`)
with either absolute `events` or relative `steps`. They are validated for allowed
keys, ordered times, balanced press/release and safe checkpoint names.

**Checkpoints** are DOSBox-X *raw* screenshots (F12+Ctrl+P). Each is the exact
indexed VGA frame (mode 13h appears as 640×400 with every pixel doubled) plus its
palette. `frame_sha256` covers the dimensions, palette and indices. Raw captures
need running emulation, because DOSBox-X renders nothing while paused. `video`
records a lossless ZMBV AVI at the VGA refresh (about 70.09 fps) with 48 kHz PCM
audio that includes the PC speaker.

**Save states are not used.** In this build, saving a state while the game holds a
DOS file open aborts the emulator (`*** buffer overflow detected ***`). This
happens in every gameplay save we tried. The cause is in `POD_Save_DOS_Files`
(`src/dos/dos_files.cpp`): `alloca(strlen(name))` followed by a `strcpy` of one
more byte, which Ubuntu's `_FORTIFY_SOURCE` build detects. Machine-state snapshots
therefore need a patched emulator. That is a follow-up, not part of this pin.

## Sources of nondeterminism (measured)

| Source | Control or observation |
|---|---|
| Host-timed keyboard input | XTest key events reach the game in host time. DOSBox-X has no emulated-time input scheduler, and its `AUTOTYPE` sleeps on a host thread (`src/gui/sdl_mapper.cpp`, class `Typer`). **Measured:** open-loop replays of the exploration and reference traces diverged at the first gameplay checkpoint, and neither reproduced level completion. The exploration replay never collapsed the building. The reference replay collapsed it but never met the bonus objective. |
| DOS clock / random seed | The BIOS tick count starts from host time at boot. AUTOEXEC sets a fixed date and time. The game seeds Turbo Pascal `Randomize` from DOS time (`0x1a62f`, called from `0x12597`) and times itself with DOS time, so both now depend only on emulated time. |
| Typewriter screens | Menus, announcements and help pages type out character by character. A key press during typing only completes the text. A checkpoint taken while typing can differ by one glyph (observed: a 76-pixel difference in the last digit of the level announcement). |
| Frame-counter outcomes | A falling bonus tile can shatter when `(frame counter + record index) mod 6 > 2` on impact (`0x145fa`). Observed: the objective ball broke in one building collapse and survived the next. |
| Emulation pacing | Fixed cycles keep emulated time consistent. Host load only changes how emulated time maps to host time, which matters only through input timing. |

**Reproducible:** screens that wait for input. The [boot smoke trace](../emulator/traces/boot-smoke.json)
covers the completed menu, the level-1 announcement, and a level-1 frame taken
400 ms in, before anything moves. With its timing, every run so far has produced
identical digests for all three checkpoints: three manual runs, five recorded runs
and every test-suite execution. An earlier variant waited only 2.5 s before the
announcement checkpoint and differed in 1 of 3 runs, on the unfinished final glyph. `tests/test_run_original.py` replays the trace whenever the pinned
emulator, Xvfb and authenticated originals are present, and otherwise reports a
skip.

## Observed behavior of the original

The game runs entirely from its own keyboard handler at `0x11091`, installed on
INT 9. On each make or break code it sets or clears one byte per control at
DS:1B78..1B81, then chains to the BIOS. A control is level-triggered. A tap
shorter than one game frame can be lost (observed with 80 ms fire taps), so
traces hold keys for 150 ms or more. That stays below the typematic delay, which
would repeat the key.

| Action | Player 1 | Player 2 | Evidence |
|---|---|---|---|
| Left / right | Z / X | ← / → | observed (both players, simultaneously) |
| Down (drop through platforms, activate special platforms) | C | ↓ | observed for P1 (drops through the purple one-way platforms) |
| Jump | M | ↑ | observed (both players) |
| Fire | N | keypad 0 | observed. Scancode `0x52`, so Insert is the same key in the handler (code only, not observed). |
| Change bomb type | Z+X held for more than 4 frames, then released | ← + → | observed for P1 (blue → green and back). P2 shares the code at `0x16053`; not observed. |
| Abandon game | Esc | | observed: GAME OVER and final score, then back to the menu |
| Background on/off | S | | observed (night-city scenery disappears) |
| Playfield width | E narrower, R wider | | observed in one-player mode |

- **Menu.** The keys are 1 (one player), 2 (two players), I (info), Z (instructions),
  R (records), L (toggle Italian/English) and Esc (exit to DOS, which closes the
  emulator). Every launch started in Italian. The first key press during typing only
  completes the menu text.
- **Bombs.** At a new game the counts are 200 / 20 / 6 / 0 for the four types
  (`0x12efd`); the HUD showed 99 for the 200 blue bombs. A thrown bomb inherits 1.5× the player's
  horizontal speed plus an upward kick, so a standing throw goes straight up and
  falls back on the thrower. This was observed and costs energy. Blasts push
  structure tiles and debris (`0x13fa6`, `0x145fa`).
- **Level 1.** The HUD starts at 01 bonus (the yellow ball, tile 108) and 50%
  destruction. The pillars do not block movement. In three attempts, a green bomb
  exploding 3–5 tiles right of the left pillar pushed the pillar aside and collapsed
  the whole building into boulders, and destruction reached 00. When both counters reach 00 the box turns white. Then "LIVELLO
  COMPLETATO" appears with destruction and bomb bonuses, fire leads to "PREPARATI
  PER IL LIVELLO 2", and level 2 shows 03 gems and 60%.
- **How the bonus objective was met.** In both completed runs the counter went
  01 → 00 only after a bomb exploded near the right wall and a yellow-ball bonus
  appeared there. That is consistent with `0x145fa` turning certain settled debris
  into a random bonus in 107–111, a range that includes 108. The original ball was
  still buried, or had shattered. This is an observation about how level 1 was
  finished here, not a claim about the intended route.
- **Death.** Energy drops on monster contact and from your own bombs. When it
  runs out a life icon disappears. With the objective ball intact, the player
  reappeared as an outline at the level start, and fire re-entered with the level
  state kept (exploration run). With the ball destroyed, the game went straight back
  to the level-1 announcement and restarted the level (reference run).
- **Two players.** The screen splits into two viewports: Player 1's red character
  on the left and Player 2's blonde character on the right. Each has its own energy,
  score, lives and bombs, around a shared objective box. Simultaneous input works.
- **Info, records and persistence.** The info screen is the shareware notice. The
  records screen lists seven 10000-point entries; names are not reproduced here.
  No run beat 10000, so `RECS.DAT` stayed unchanged in every run copy, and
  high-score entry and persistence are **not yet observed**.

## Reference runs

Summaries are in [`analysis/original-runs/`](../analysis/original-runs/) and the matching
input traces in [`emulator/traces/`](../emulator/traces/). Every run used the pinned
emulator and configuration. Every run re-verified all 14 original files after exit,
ended with emulator status 0, and left the run copy unchanged.

| Summary | What it shows | Unpaused game time | Checkpoints |
|---|---|---|---|
| `reference-level1.json` | One player, Italian. Collapse (destruction 50 → 00), the ball shatters, death, automatic level restart, a second collapse, bonus objective met (between `r2-k-c` and `r2-drain-1`), level-complete tally, level-2 announcement and start, S/E/R keys, Esc, info and records screens, exit | 263.6 s | 62 |
| `exploration-level1.json` (run label `smoke`) | The first completion of level 1, plus bomb and control experiments, death with fire re-entry, level 2, Esc, exit | 184.3 s | 113 |
| `two-player-controls.json` | Two-player split screen, simultaneous P1/P2 movement and jumps, P2 keypad-0 fire, two-player game over | 58.2 s | 9 |
| `boot-smoke.json` | Reproducible boot frames, recorded from a clean checkout of the commit that added the tool | 15.1 s | 3 |

The three closed-loop sessions were recorded before the tool's first commit
(`tool_sha256` is null). The later edits changed session finalization, request
validation and provenance fields, not how keys or checkpoints are sent. Their
gameplay frame digests identify local captures. They are not a replay oracle. The
checkpoint named `menu-english` in the reference run is misleading: the L key only
completed the typewriter text, and that run stayed in Italian.

## Level and mode inventory

| Level | Size (tiles) | Bonus objective (tile × count) | Destruction % (header byte) | Observed |
|---|---|---|---|---|
| 1 | 60×33 | 108 × 1 | 50 | played and completed (2 runs), in 1P and in a 2P start |
| 2 | 100×53 | 110 × 3 | 60 | announced and entered. HUD matched the header. Not played. |
| 3 | 150×60 | 108 × 7 | 20 | not observed |
| 4 | 100×58 | 109 × 3 | 70 | not observed |
| 5 | 110×62 | 107 × 8 | 65 | not observed |
| 6 | 180×64 | 111 × 8 | 40 | not observed |
| 7 | 140×52 | 106 × 1 | 10 | not observed |

The resource docs keep the header's percentage byte opaque. It matched the HUD
destruction value in levels 1 and 2. The level counter (DS:79B7) is set to 1 for a
new game and incremented by the level-complete tally (`0x11d61`). Above 7, the main
program ends the game through `0x11b14` mode 2. Level 7 loads `PROVA.SPR` and
`GRAN.MST` instead of `BOMOMIMK.SPR` (`0x12adc`). Tile 69, which the down key
checks for special platforms and telepods, occurs only in levels 3–6. So all seven
stored levels appear reachable in order in this shareware release, but levels 3–7,
the end-of-game screen and the level-7 special content are **untested**.

Modes observed: one player, two players, Italian and English menus, background
toggle, and playfield width (one player). Not observed: completing any level in
two-player mode, high-score entry and `RECS.DAT` persistence, levels 3–7, the end
of the game, judging sound quality (only captured), and any other CPU, cycles or
machine setting.

## Follow-ups this baseline enables

- An emulated-time input injector and a fixed save-state path, as a patched and
  separately pinned DOSBox-X build. This is needed for exact original-versus-rebuild
  replays (#13) and for memory-state checkpoints (#4, #5).
- Recording level-load memory, for the level-decoder tails (#5), once state capture
  works.
- Reaching levels 3–7 and a hall-of-fame entry, to complete the inventory (#14).
