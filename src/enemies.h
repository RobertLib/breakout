#pragma once

#include "globals.h"
#include "enemy.h"

extern int numEnemies;

extern Enemy *enemies;

void initializeEnemies(void);

void updateEnemies(void);

void drawEnemies(void);

void destroyEnemies(void);
