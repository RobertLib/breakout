#include "bullet.h"
#include "../bricks/bricks.h"
#include "../enemies/enemies.h"
#include "../lib/camera.h"
#include "../lib/gfx.h"
#include "../lib/particles.h"
#include "../paddle/paddle.h"

static const int BULLET_HEIGHT = 16;
static const int SPEED = 700;

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

  Brick *nearBricks[32];
  int n = queryBricksNear(bullet->pos.x, bullet->pos.y, nearBricks, 32);

  for (int i = 0; i < n; i++)
  {
    Brick *brick = nearBricks[i];

    if (checkCollision(
            bullet->pos.x,
            bullet->pos.y,
            BULLET_WIDTH,
            BULLET_HEIGHT,
            brick->pos.x,
            brick->pos.y,
            BRICK_WIDTH,
            BRICK_HEIGHT))
    {
      bullet->active = false;
      damageBrick(brick, 1, false);
      spawnGlowPuff(bullet->pos.x + BULLET_WIDTH / 2.0f, bullet->pos.y,
                    (SDL_Color){255, 150, 120, 255}, 30, 0.2f);
      break;
    }
  }

  if (!bullet->active)
  {
    return;
  }

  // Check enemy hits
  for (int i = 0; i < MAX_ENEMIES; i++)
  {
    Enemy *enemy = &enemies[i];

    if (!enemy->active)
    {
      continue;
    }

    int size = enemySize(enemy->type);

    if (checkCollision(
            bullet->pos.x,
            bullet->pos.y,
            BULLET_WIDTH,
            BULLET_HEIGHT,
            enemy->pos.x,
            enemy->pos.y,
            size,
            size))
    {
      bullet->active = false;
      killEnemy(enemy, 400);
      break;
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

  float screenY = bullet->pos.y - camera.y;

  // Glow around the laser bolt
  SDL_SetTextureColorMod(texGlow, 255, 110, 90);
  SDL_SetTextureAlphaMod(texGlow, 160);
  SDL_FRect glow = {
      bullet->pos.x + BULLET_WIDTH / 2.0f - 14,
      screenY + BULLET_HEIGHT / 2.0f - 16,
      28,
      32};
  SDL_RenderTexture(renderer, texGlow, NULL, &glow);
  SDL_SetTextureColorMod(texGlow, 255, 255, 255);
  SDL_SetTextureAlphaMod(texGlow, 255);

  // Bright core
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 255, 230, 220, 255);
  SDL_FRect core = {
      bullet->pos.x + BULLET_WIDTH / 2.0f - 2,
      screenY,
      4,
      BULLET_HEIGHT};
  SDL_RenderFillRect(renderer, &core);
}
