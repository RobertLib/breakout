#pragma once

#include "../globals.h"

static const int ENEMY_SIZE = 20;

typedef struct Enemy
{
  Vec2 pos;
  Vec2 vel;
  int speed;
  bool active;
  Timer changeOfDirectionTimer;
} Enemy;

void initializeEnemy(Enemy *enemy, const float x, const float y);

void updateEnemy(Enemy *enemy);

void drawEnemy(const Enemy *enemy);

void destroyEnemy(const Enemy *enemy);
