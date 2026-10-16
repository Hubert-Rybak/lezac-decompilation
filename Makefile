CC ?= cc
AR ?= ar
PYTHON ?= python3
CFLAGS ?= -std=c99 -O2 -Wall -Wextra -Werror -pedantic

.PHONY: all test verify-evidence original pascal-native pascal-dos game lockstep
all: build/liblezac.a build/liblezac.so

pascal-native:
	$(PYTHON) tools/build_pascal.py pascal/LZBRIDGE.PAS build/pascal-native/liblezac_pascal.so
	$(PYTHON) tools/build_pascal.py pascal/LZRLEBR.PAS build/pascal-native/lzrle.so
	$(PYTHON) tools/build_pascal.py pascal/LZPROBE.PAS build/pascal-native/lzprobe
	build/pascal-native/lzprobe

pascal-dos:
	$(PYTHON) tools/build_pascal.py pascal/LZPROBE.PAS build/pascal-dos/LZPROBE.EXE --target dos
	$(PYTHON) tools/run_dos_probe.py build/pascal-dos/LZPROBE.EXE

# The reconstructed game, compiled from pascal/ only (see docs/reconstruction.md).
game:
	$(PYTHON) tools/build_game.py

# Original and rebuild in lockstep under the deterministic machine (needs original/).
lockstep: game
	LEZAC_GAMEPLAY_TESTS=1 $(PYTHON) -m pytest tests/test_lockstep.py -q

build:
	mkdir -p build

build/lezac.o: recovered/lezac.c recovered/lezac.h | build
	$(CC) $(CFLAGS) -fPIC -c recovered/lezac.c -o $@

build/liblezac.a: build/lezac.o
	$(AR) rcs $@ $<

build/liblezac.so: build/lezac.o
	$(CC) -shared $< -o $@

original:
	$(PYTHON) tools/fetch_original.py

test:
	npm test
	$(PYTHON) -m pytest tests -q

verify-evidence:
	$(PYTHON) -m pytest tests/test_evidence.py tests/test_summary.py -q
