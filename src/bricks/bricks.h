#pragma once

#include "brick.h"
#include "../globals.h"

extern int numBricks;

extern Brick *bricks;

// Collects active bricks in the 3x3 grid cells around (x, y).
// Returns the number of bricks written to out (up to maxOut).
int queryBricksNear(float x, float y, Brick **out, int maxOut);

// Applies damage to a brick, handling scoring, effects, drops, explosions
// and the level-complete check. Solid bricks shrug it off.
// byBall: true when the hit came from a ball (combo scoring applies).
void damageBrick(Brick *brick, int damage, bool byBall);

void initializeBricks(void);

void updateBricks(void);

void drawBricks(void);

void destroyBricks(void);
