#pragma once

#include "../globals.h"

static const int BALL_SIZE = 20;

typedef struct Ball
{
  Vec2 pos;
  Vec2 vel;
  int speed;
  bool active;
  Vec2 dockedAt;
} Ball;

bool isBallDocked(const Ball *ball);

void releaseBall(Ball *ball);

void initializeBall(Ball *ball, bool active);

void updateBall(Ball *ball);

void drawBall(const Ball *ball);
