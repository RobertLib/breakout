#pragma once

#include "brick-item.h"
#include "../globals.h"

static const int BRICK_WIDTH = 50;

extern const int BRICK_HEIGHT;

typedef enum BrickType
{
  _, // Empty
  B, // Brick
  S, // Solid
  F, // Final
  E  // Enemy
} BrickType;

typedef struct BrickProperty
{
  SDL_Color color;
  int value;
} BrickProperty;

typedef struct Brick
{
  Vec2 pos;
  BrickType type;
  bool active;
  SDL_Color color;
  int value;
  BrickItem *brickItem;
} Brick;

void initializeBrickProperties(void);

void initializeBrick(
    Brick *brick,
    const float x,
    const float y,
    const BrickType type);

void updateBrick(Brick *brick);

void drawBrick(const Brick *brick);

void destroyBrick(Brick *brick);
