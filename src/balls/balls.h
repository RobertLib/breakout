#pragma once

#include "ball.h"
#include "../globals.h"

#define BALL_COUNT 3

extern Ball balls[BALL_COUNT];

extern SDL_Texture *ballTexture;

extern MIX_Audio *ballSound;

void activateAllBalls(void);

void resetBalls(void);

bool isBallsDocked(void);

void releaseBalls(void);

void initializeBalls(void);

void updateBalls(void);

void drawBalls(void);

void destroyBalls(void);
