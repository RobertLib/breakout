#pragma once

#include "level-types.h"

// One level by 0-based index, and never NULL: an index outside 0..numLevels-1
// gets an empty level named LEVEL instead.
//
// paddle.level reaches the bricks, the enemies, the HUD and the game over
// screen, and it arrives from a save file, from BREAKUP_LEVEL and from the
// level select grid. Each of those used to index the levels array directly,
// so the one place a bad value could be caught was five places that did not -
// which is why the array itself is no longer handed out.
const Level *getLevel(int index);

// Get number of levels
int getNumberOfLevels(void);

// Get the Y pixel position of the bottom of the last non-empty row for a given level
int getLevelContentBottom(int levelIndex);

// Initialize level manager (loads all level txt files)
void initializeLevelManager(void);

// Cleanup level manager
void destroyLevelManager(void);
