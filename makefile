CC = clang
OBJ_NAME = ./build/breakout
INCLUDE_PATHS = -I/opt/homebrew/include
LIBRARY_PATHS = -L/opt/homebrew/lib
COMPILER_FLAGS = -std=c17 -Wall -Wextra -Wpedantic -O2
LINKER_FLAGS = -lSDL2 -lSDL2_ttf -lSDL2_mixer
OBJS = $(shell find ./src -name '*.c')

all:
	$(CC) -o $(OBJ_NAME) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(OBJS) # -g -fsanitize=address
