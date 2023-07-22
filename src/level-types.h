#pragma once

#define LEVEL_PATTERN_COLS 15
#define LEVEL_PATTERN_ROWS 150
#define LEVEL_PATTERN_LENGTH (LEVEL_PATTERN_COLS * LEVEL_PATTERN_ROWS)
#define LEVEL_PATTERN_INDENT 11
#define LEVEL_PATTERN_SPACING 2
#define LEVEL_PATTERN_ROW_HEIGHT 20
// Vertical start of the pattern: status bar height (46) plus one empty brick
// row, so the top of a level never sits flush against the HUD
#define LEVEL_PATTERN_TOP (46 + LEVEL_PATTERN_ROW_HEIGHT + LEVEL_PATTERN_SPACING)
#define LEVEL_HEIGHT (LEVEL_PATTERN_TOP + LEVEL_PATTERN_ROWS * (LEVEL_PATTERN_ROW_HEIGHT + LEVEL_PATTERN_SPACING))

#define LEVEL_NAME_MAX 24

// Pattern cell characters:
//   _ or space  empty
//   B  basic brick (1 hit)
//   D  durable brick (2 hits)
//   T  tough brick (3 hits)
//   S  solid brick (indestructible)
//   X  explosive brick (blows up its neighborhood)
//   G  gold brick (bonus points)
//   F  final crystal (destroy all of these to win the level)
//   E  drifter enemy spawn point
//   V  diver enemy spawn point
//   W  splitter enemy spawn point
//   U  shooter (saucer) enemy spawn point
typedef struct Level
{
  char pattern[LEVEL_PATTERN_LENGTH];
  char name[LEVEL_NAME_MAX];
} Level;
