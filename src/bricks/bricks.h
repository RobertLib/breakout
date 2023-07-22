#pragma once

#include "brick.h"
#include "../balls/ball.h"
#include "../globals.h"

extern int numBricks;

extern Brick *bricks;

extern SDL_Texture *bricksTexture;
extern SDL_Texture *brickItemsTexture;

typedef void (*SurroundingTilesCallback)(Obj *obj, Brick *brick);

void checkSurroundingTilesInGrid(
    Obj *obj,
    SurroundingTilesCallback surroundingTilesCallback);

bool areAllFinalBricksInactive(void);

void initializeBricks(void);

void updateBricks(void);

void drawBricks(void);

void destroyBricks(void);
