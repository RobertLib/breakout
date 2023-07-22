#include "enemy.h"
#include "enemies.h"
#include "../bricks/bricks.h"
#include "../lib/camera.h"
#include "../paddle/paddle.h"

static void changeOfDirectionCallback(Obj *obj);

static void randomizeVelocity(Enemy *enemy)
{
  enemy->vel.x = 0;
  enemy->vel.y = 0;

  while (fabsf(enemy->vel.x) < 0.25 || fabsf(enemy->vel.y) < 0.25)
  {
    enemy->vel.x = 2 * (float)rand() / RAND_MAX - 1;
    enemy->vel.y = 2 * (float)rand() / RAND_MAX - 1;
  }

  vec2Norm(enemy->vel, 1);
}

void initializeEnemy(Enemy *enemy, const float x, const float y)
{
  enemy->pos.x = x;
  enemy->pos.y = y;
  enemy->speed = 100;
  enemy->active = true;

  randomizeVelocity(enemy);

  enemy->changeOfDirectionTimer.elapsedTime = (float)rand() / RAND_MAX * 5;
  enemy->changeOfDirectionTimer.duration = 5;
  enemy->changeOfDirectionTimer.timeoutCallback = &changeOfDirectionCallback;
  enemy->changeOfDirectionTimer.obj = (Obj *)enemy;
}

static void handleCollisionWithBrick(Enemy *enemy, const char direction)
{
  if (direction == 'x')
  {
    enemy->vel.x *= -1;
  }
  else if (direction == 'y')
  {
    enemy->vel.y *= -1;
  }
}

static void handleSurroundingTiles(Obj *enemy, Brick *brick)
{
  if (checkCollision(
          enemy->pos.x + enemy->vel.x * enemy->speed * dt,
          enemy->pos.y,
          ENEMY_SIZE,
          ENEMY_SIZE,
          brick->pos.x,
          brick->pos.y,
          BRICK_WIDTH,
          BRICK_HEIGHT))
  {
    handleCollisionWithBrick((Enemy *)enemy, 'x');
  }

  if (checkCollision(
          enemy->pos.x,
          enemy->pos.y + enemy->vel.y * enemy->speed * dt,
          ENEMY_SIZE,
          ENEMY_SIZE,
          brick->pos.x,
          brick->pos.y,
          BRICK_WIDTH,
          BRICK_HEIGHT))
  {
    handleCollisionWithBrick((Enemy *)enemy, 'y');
  }
}

void updateEnemy(Enemy *enemy)
{
  if (!enemy->active)
  {
    return;
  }

  float nextX = enemy->pos.x + enemy->vel.x * enemy->speed * dt;
  float nextY = enemy->pos.y + enemy->vel.y * enemy->speed * dt;

  // Check if the enemy is out of the screen
  if (nextX < 0 || nextX + ENEMY_SIZE > SCREEN_WIDTH)
  {
    enemy->vel.x *= -1;
  }

  if (nextY < camera.y)
  {
    enemy->vel.y *= -1;
  }

  checkSurroundingTilesInGrid((Obj *)enemy, handleSurroundingTiles);

  // If the enemy is outside the lower border of the camera, it will be disabled
  if (enemy->pos.y > camera.y + SCREEN_HEIGHT)
  {
    enemy->active = false;
  }

  enemy->pos.x += enemy->vel.x * enemy->speed * dt;
  enemy->pos.y += enemy->vel.y * enemy->speed * dt;

  updateTimer(&enemy->changeOfDirectionTimer);
}

void drawEnemy(const Enemy *enemy)
{
  if (!enemy->active)
  {
    return;
  }

  SDL_FRect dstRect = {enemy->pos.x, enemy->pos.y - camera.y, ENEMY_SIZE, ENEMY_SIZE};
  SDL_RenderTexture(renderer, enemyTexture, NULL, &dstRect);
}

void destroyEnemy(UNUSED const Enemy *enemy)
{
}

// Callbacks

static void changeOfDirectionCallback(Obj *obj)
{
  randomizeVelocity((Enemy *)obj);
}
