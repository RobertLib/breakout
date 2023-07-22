#pragma once

#include "../globals.h"
#include "../paddle/paddle.h"

static const int BRICK_ITEM_WIDTH = 40;
static const int BRICK_ITEM_HEIGHT = 15;

typedef struct BrickItem
{
  Vec2 pos;
  bool active;
  PaddleType type;
  float timer;
} BrickItem;

BrickItem *newBrickItem(const float x, const float y, const PaddleType type);

void updateBrickItem(BrickItem *brickItem);

void drawBrickItem(BrickItem *brickItem);

void destroyBrickItem(BrickItem *brickItem);
