#pragma once

#include "bricks/brick.h"

static const int LEVEL_PATTERN_COLS = 15;
static const int LEVEL_PATTERN_ROWS = 150;
static const int LEVEL_PATTERN_LENGTH = LEVEL_PATTERN_COLS * LEVEL_PATTERN_ROWS;
static const int LEVEL_PATTERN_INDENT = 11;
static const int LEVEL_PATTERN_SPACING = 2;
static const int LEVEL_PATTERN_ROW_HEIGHT = 20;
static const int LEVEL_HEIGHT = LEVEL_PATTERN_INDENT + LEVEL_PATTERN_ROWS * (LEVEL_PATTERN_ROW_HEIGHT + LEVEL_PATTERN_SPACING);
static const int NUMBER_OF_LEVELS = 2;

typedef struct Level
{
  BrickType pattern[LEVEL_PATTERN_LENGTH];
} Level;

static const Level LEVELS[NUMBER_OF_LEVELS] = {
    {{
        S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,F,F,_,_,_,_,_,_,_,
        _,_,_,_,_,F,_,_,F,_,_,_,_,_,_,
        _,_,_,_,_,F,_,_,F,_,_,_,_,_,_,
        _,_,_,_,_,_,F,F,_,_,_,_,_,_,_,
        _,_,_,_,_,F,_,_,F,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,E,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,S,S,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,E,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,B,B,B,B,_,_,_,_,_,_,_,_,
        _,_,_,B,_,_,B,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,E,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,B,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,B,S,B,S,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,E,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        S,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        S,_,_,_,_,_,B,B,B,B,B,B,_,_,_,
        S,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        S,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,E,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,S,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,B,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,B,B,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,B,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,S,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,B,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,E,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,S,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,S,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,S,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,E,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,S,S,_,S,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,E,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,B,B,B,S,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,E,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,B,B,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,S,S,S,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,E,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,S,B,_,_,_,
        _,S,B,S,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,E,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,B,_,B,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,E,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,S,S,S,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,E,_,_,
        _,_,_,_,_,_,B,_,B,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,S,B,S,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,B,B,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,E,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
    }},
    {{
        S,S,S,S,S,S,S,S,S,S,S,S,S,S,S,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,F,F,_,_,_,_,_,_,_,
        _,_,_,_,_,F,_,_,F,_,_,_,_,_,_,
        _,_,_,_,_,F,_,_,F,_,_,_,_,_,_,
        _,_,_,_,_,_,F,F,_,_,_,_,_,_,_,
        _,_,_,_,_,F,_,_,F,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        B,_,_,_,_,_,_,_,_,_,_,_,_,_,B,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
    }}};
