CC = clang
OBJ_NAME = ./build/breakout
INCLUDE_PATHS = -I/opt/homebrew/include -I/usr/local/include
LIBRARY_PATHS = -L/opt/homebrew/lib -L/usr/local/lib
COMPILER_FLAGS_BASE = -std=c17 -Wall -Wextra -Wpedantic
LINKER_FLAGS = -lSDL3 -lSDL3_ttf -lSDL3_mixer -Wl,-rpath,/usr/local/lib -Wl,-rpath,/opt/homebrew/lib -Wl,-headerpad_max_install_names
OBJS = $(shell find ./src -name '*.c')

# Debug build (default) - includes editor
COMPILER_FLAGS_DEBUG = $(COMPILER_FLAGS_BASE) -O0 -g -DDEBUG

# Release build - no editor, optimized
COMPILER_FLAGS_RELEASE = $(COMPILER_FLAGS_BASE) -O2 -DNDEBUG

# Default to debug build
all: debug

# Debug build target
debug: COMPILER_FLAGS = $(COMPILER_FLAGS_DEBUG)
debug: embed
	@echo "Building DEBUG version (with editor)..."
	$(CC) -o $(OBJ_NAME) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS)

# Release build target
release: COMPILER_FLAGS = $(COMPILER_FLAGS_RELEASE)
release: embed
	@echo "Building RELEASE version (no editor)..."
	$(CC) -o $(OBJ_NAME) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS)

embed:
	./embed_assets.sh

clean:
	rm -f $(OBJ_NAME) src/embedded_assets.h

bundle-debug:
	./create_app_bundle.sh debug

bundle-release:
	./create_app_bundle.sh release

bundle: bundle-release

.PHONY: all debug release embed clean bundle-debug bundle-release bundle
