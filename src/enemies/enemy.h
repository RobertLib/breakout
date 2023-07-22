#pragma once

#include "../globals.h"
#include "../types.h"

typedef struct Enemy
{
  Vec2 pos;
  Vec2 vel; // normalized direction
  float speed;
  bool active;
  EnemyType type;
  float animTime;
  float actionTimer;  // direction change / dive delay / shoot cooldown
  float spawnFade;    // 0..1 fade-in after spawning
  int spawnerIndex;   // owning spawn point, -1 for splitter minis
} Enemy;

// Pixel size of an enemy of the given type
int enemySize(EnemyType type);

void initializeEnemy(Enemy *enemy, EnemyType type, float x, float y, int spawnerIndex);

void updateEnemy(Enemy *enemy);

void drawEnemy(const Enemy *enemy);
