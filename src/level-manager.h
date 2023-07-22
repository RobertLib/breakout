#pragma once

#include "level-types.h"
#include <stdbool.h>

// Load levels from file (embedded or external)
bool loadLevelsFromFile(const char *filename);

// Save levels to file
bool saveLevelsToFile(const char *filename);

// Get current levels array
Level* getLevels(void);

// Get number of levels
int getNumberOfLevels(void);

// Set a specific level
void setLevel(int index, const Level *level);

// Add a new level
bool addLevel(const Level *level);

// Initialize level manager (loads default or file levels)
void initializeLevelManager(void);

// Cleanup level manager
void destroyLevelManager(void);
