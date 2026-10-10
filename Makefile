CC ?= cc
AR ?= ar
PYTHON ?= python3
CFLAGS ?= -std=c99 -O2 -Wall -Wextra -Werror -pedantic

.PHONY: all test verify-evidence original
all: build/liblezac.a build/liblezac.so

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
