#pragma once

#include "enemy.h"
#include "../globals.h"

#define MAX_ENEMIES 24
#define MAX_ENEMY_BULLETS 8

typedef struct EnemyBullet
{
  Vec2 pos;
  Vec2 vel;
  bool active;
} EnemyBullet;

extern Enemy enemies[MAX_ENEMIES];
extern EnemyBullet enemyBullets[MAX_ENEMY_BULLETS];

// Kills an enemy: explosion, score, splitter spawns minis, spawner respawn
void killEnemy(Enemy *enemy, int score);

// An enemy left the play area without dying (starts the spawner respawn timer)
void enemyDespawned(Enemy *enemy);

// Used by explosive bricks
void killEnemiesInRadius(float x, float y, float radius);

// Fired by shooter enemies
void spawnEnemyBullet(float x, float y);

void clearEnemyBullets(void);

void initializeEnemies(void);

void updateEnemies(void);

void drawEnemies(void);

void destroyEnemies(void);
