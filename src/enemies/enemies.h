#pragma once

#include "enemy.h"
#include "../globals.h"

extern int numEnemies;

extern Enemy *enemies;

extern SDL_Texture *enemyTexture;

void initializeEnemies(void);

void updateEnemies(void);

void drawEnemies(void);

void destroyEnemies(void);
