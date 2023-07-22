#pragma once

#include "../globals.h"

static const int BULLET_WIDTH = 10;

typedef struct Bullet
{
  Vec2 pos;
  bool active;
} Bullet;

void initializeBullet(Bullet *bullet);

void updateBullet(Bullet *bullet);

void drawBullet(const Bullet *bullet);
