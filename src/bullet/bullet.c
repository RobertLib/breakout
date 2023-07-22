#include "bullet.h"
#include "../bricks/bricks.h"
#include "../lib/camera.h"
#include "../paddle/paddle.h"

static const int BULLET_HEIGHT = 20;
static const int SPEED = 600;

void initializeBullet(Bullet *bullet)
{
  bullet->pos.x = 0;
  bullet->pos.y = 0;
  bullet->active = false;
}

void updateBullet(Bullet *bullet)
{
  if (!bullet->active)
  {
    return;
  }

  bullet->pos.y -= SPEED * dt;

  for (int i = 0; i < numBricks; i++)
  {
    if (!bricks[i].active)
    {
      continue;
    }

    // Check if the bullet collides with a brick
    if (checkCollision(
            bullet->pos.x,
            bullet->pos.y,
            BULLET_WIDTH,
            BULLET_HEIGHT,
            bricks[i].pos.x,
            bricks[i].pos.y,
            BRICK_WIDTH,
            BRICK_HEIGHT))
    {
      if (bricks[i].type != S)
      {
        bricks[i].active = false;
      }

      bullet->active = false;

      if (areAllFinalBricksInactive())
      {
        paddle.levelCompleted = true;
      }
    }
  }

  // If the bullet is out of the screen, deactivate it
  if (bullet->pos.y + BULLET_HEIGHT < camera.y)
  {
    bullet->active = false;
  }
}

void drawBullet(const Bullet *bullet)
{
  if (!bullet->active)
  {
    return;
  }

  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

  SDL_RenderFillRect(
      renderer,
      &(SDL_Rect){
          bullet->pos.x,
          bullet->pos.y - camera.y,
          BULLET_WIDTH,
          BULLET_HEIGHT});
}
