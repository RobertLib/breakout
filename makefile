TARGET := breakup
SRC_DIR := src
BUILD_DIR := build

UNAME_S := $(shell uname -s)
SDL_PKGS := sdl3 sdl3-ttf sdl3-mixer

# `=` rather than `:=`, so pkg-config runs only when a build actually asks for
# these. The three platform scripts in packaging/ pass CFLAGS and LDFLAGS whole
# on the command line, where they override even a target-specific assignment,
# and neither a mingw cross-build nor a Linux one has a pkg-config that knows
# about this machine's SDL3. Immediate assignment ran it regardless — on every
# invocation of every target, including `make clean` — and printed its complaint
# into builds with no use for the answer.
CFLAGS_BASE = -std=c17 -Wall -Wextra -Wpedantic $(shell pkg-config --cflags $(SDL_PKGS))
LDFLAGS_SDL = $(shell pkg-config --libs $(SDL_PKGS))

# The development link line, and the one place in this file that is about a
# platform. `-headerpad_max_install_names` and the Homebrew rpaths are ld64
# flags: GNU ld rejects the first outright, so a Linux developer running plain
# `make` got a link error about a flag that has nothing to do with their machine.
ifeq ($(UNAME_S),Darwin)
# `?=` cannot do this: make defines CC itself, so its origin is `default` rather
# than undefined and `?=` leaves it alone — which is why this said clang and
# built with cc. Elsewhere make's own `cc` is the right answer anyway, and the
# cross-build passes CC on the command line.
ifeq ($(origin CC),default)
CC := clang
endif
# Homebrew's own .pc files already carry an rpath to wherever they were
# installed, so adding both of these unconditionally handed ld64 the same
# -rpath twice and it said so on every single link. `filter-out` drops whichever
# one pkg-config has already asked for and keeps the other, which is the point
# of naming both: /usr/local for an Intel Homebrew, /opt/homebrew for an
# Apple-silicon one, and neither is guaranteed to be the one that answered.
MAC_RPATHS := -Wl,-rpath,/usr/local/lib -Wl,-rpath,/opt/homebrew/lib
LDFLAGS = $(LDFLAGS_SDL) $(filter-out $(LDFLAGS_SDL),$(MAC_RPATHS)) \
          -Wl,-headerpad_max_install_names
else
# CC is left to make's own default of `cc`, which is the right answer on Linux;
# `?=` could not have set it anyway, per the note above.
LDFLAGS = $(LDFLAGS_SDL) -lm
endif

SOURCES := $(shell find $(SRC_DIR) -name '*.c')
OBJECTS_REL := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/release/%.o,$(SOURCES))
OBJECTS_DBG := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/debug/%.o,$(SOURCES))

# Header dependencies. These go in the compile recipe rather than into CFLAGS,
# because CFLAGS is assigned per-target below and passed whole on the command
# line by the three packaging scripts — either of which would drop them, and a
# release build is exactly where a stale object matters most.
#
# Without these, an object depended on its .c file and nothing else. Editing a
# header rebuilt nothing, and the sharp edge was assets/levels/*.txt: `make`
# regenerated the embedded header, level-manager.c was not recompiled, and the
# binary went on carrying the levels from before the edit. `make clean` was the
# only honest way to build and nothing said so.
#
# -MP writes a phony target for every header, so deleting or renaming one is a
# rebuild rather than "no rule to make target".
DEPFLAGS := -MMD -MP

DEPS := $(OBJECTS_REL:.o=.d) $(OBJECTS_DBG:.o=.d)
-include $(DEPS)

# The generated header every object may include, as an order-only prerequisite:
# it has to exist before anything compiles, but it is not a reason to recompile
# (the depfiles above decide that). Without this, `make -j` raced embed_assets.sh
# against the compiles that include what it writes, and the packaging scripts had
# to run `make embed` on its own first to be safe.
$(OBJECTS_REL) $(OBJECTS_DBG): | embed

# Default to debug build
all: debug

# Debug build target
debug: CFLAGS = $(CFLAGS_BASE) -O0 -g -DDEBUG
debug: $(TARGET)_dbg
	@echo "Debug build ready: $(TARGET)_dbg"

$(TARGET)_dbg: $(OBJECTS_DBG)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/debug/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Release build target
release: CFLAGS = $(CFLAGS_BASE) -O2 -DNDEBUG
release: $(TARGET)
	@echo "Release build ready: $(TARGET)"

$(TARGET): $(OBJECTS_REL)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/release/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

run: debug
	./$(TARGET)_dbg

embed:
	./embed_assets.sh

# The release, and the only thing worth handing to anybody else: a universal
# .app with SDL inside it, signed with Developer ID, notarized by Apple and
# stapled. `make debug` and `make release` above are the development loop -
# they link Homebrew's SDL3 and run on this machine only.
mac:
	packaging/build_macos.sh

# The Windows payload: breakup.exe, the three SDL DLLs beside it and nothing to
# install. Cross-built with mingw-w64, which works on this Mac as well as on
# Linux — src/ includes SDL and the C standard library and stops, so what a
# Windows build needs is a compiler that targets Windows.
win:
	packaging/build_windows.sh

# The Linux payload, and the one archive that cannot be cut on this Mac: its SDL
# is compiled against the userland it will run on. Run it on a Linux box, or
# press the button on .github/workflows/payloads.yml, which is the Linux machine
# for anybody who has not got one.
linux:
	packaging/build_linux.sh

# `press` is not a build and not a release. Every pixel of this game is drawn at
# runtime, so the store page's screenshots are captured from the built game
# rather than kept as files - which means they can be *rebuilt* after a change to
# the art instead of being re-photographed by hand and quietly left a version
# behind. See tools/press_kit.sh, and BREAKUP_SHOT in src/main.c.
press: release
	PRESS_BINARY=$(CURDIR)/$(TARGET) tools/press_kit.sh

# vendor/ is a verified download and survives a clean; this refetches it.
sdl3:
	packaging/fetch_sdl3.sh vendor

# Build output only. `dist/` is deliberately not in here: it holds the packaged
# releases and the press kit, some of which cost a notarization round trip with
# Apple to make again, and `make clean` is what somebody reaches for to force a
# rebuild — not to throw those away. `make distclean` is where that lives now.
clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TARGET)_dbg src/embedded_assets.h

# Everything clean removes, and the packaged releases with it.
distclean: clean
	rm -rf dist

.PHONY: all debug release run embed clean distclean mac win linux press sdl3
