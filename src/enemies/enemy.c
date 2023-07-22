#include "enemy.h"
#include "enemies.h"
#include "../bricks/bricks.h"
#include "../lib/camera.h"
#include "../lib/gfx.h"
#include "../paddle/paddle.h"

int enemySize(EnemyType type)
{
  return type == ENEMY_MINI ? GFX_ENEMY_MINI_SIZE : GFX_ENEMY_SIZE;
}

static void randomizeVelocity(Enemy *enemy)
{
  enemy->vel.x = 0;
  enemy->vel.y = 0;

  while (fabsf(enemy->vel.x) < 0.25f || fabsf(enemy->vel.y) < 0.25f)
  {
    enemy->vel.x = frandRange(-1, 1);
    enemy->vel.y = frandRange(-1, 1);
  }

  enemy->vel = vec2Norm(enemy->vel, 1);
}

void initializeEnemy(Enemy *enemy, EnemyType type, float x, float y, int spawnerIndex)
{
  enemy->pos.x = x;
  enemy->pos.y = y;
  enemy->type = type;
  enemy->active = true;
  enemy->animTime = frand() * 6.28f;
  enemy->spawnFade = 0;
  enemy->spawnerIndex = spawnerIndex;

  switch (type)
  {
  case ENEMY_DRIFTER:
    enemy->speed = 95;
    randomizeVelocity(enemy);
    enemy->actionTimer = frandRange(2.5f, 5.0f);
    break;
  case ENEMY_DIVER:
    enemy->speed = 60;
    randomizeVelocity(enemy);
    enemy->actionTimer = frandRange(2.0f, 4.0f); // time until the dive
    break;
  case ENEMY_SPLITTER:
    enemy->speed = 70;
    randomizeVelocity(enemy);
    enemy->actionTimer = frandRange(2.5f, 5.0f);
    break;
  case ENEMY_SHOOTER:
    enemy->speed = 80;
    enemy->vel = (Vec2){(rand() % 2) ? 1.0f : -1.0f, 0};
    enemy->actionTimer = frandRange(2.5f, 4.5f); // shoot cooldown
    break;
  case ENEMY_MINI:
    enemy->speed = 150;
    randomizeVelocity(enemy);
    enemy->actionTimer = frandRange(1.5f, 3.0f);
    break;
  default:
    break;
  }
}

static void bounceOffBricks(Enemy *enemy)
{
  int size = enemySize(enemy->type);

  Brick *nearBricks[32];
  int n = queryBricksNear(enemy->pos.x + size / 2.0f, enemy->pos.y + size / 2.0f,
                          nearBricks, 32);

  for (int i = 0; i < n; i++)
  {
    Brick *brick = nearBricks[i];

    if (checkCollision(
            enemy->pos.x + enemy->vel.x * enemy->speed * dt,
            enemy->pos.y,
            size, size,
            brick->pos.x, brick->pos.y,
            BRICK_WIDTH, BRICK_HEIGHT))
    {
      enemy->vel.x *= -1;
    }

    if (checkCollision(
            enemy->pos.x,
            enemy->pos.y + enemy->vel.y * enemy->speed * dt,
            size, size,
            brick->pos.x, brick->pos.y,
            BRICK_WIDTH, BRICK_HEIGHT))
    {
      enemy->vel.y *= -1;
    }
  }
}

void updateEnemy(Enemy *enemy)
{
  if (!enemy->active)
  {
    return;
  }

  int size = enemySize(enemy->type);

  enemy->animTime += (float)dt;
  enemy->spawnFade = fminf(1.0f, enemy->spawnFade + 2.0f * (float)dt);

  bool diving = enemy->type == ENEMY_DIVER && enemy->actionTimer <= 0;

  // Type-specific behavior
  enemy->actionTimer -= (float)dt;

  switch (enemy->type)
  {
  case ENEMY_DRIFTER:
  case ENEMY_SPLITTER:
  case ENEMY_MINI:
    if (enemy->actionTimer <= 0)
    {
      randomizeVelocity(enemy);
      enemy->actionTimer = frandRange(2.5f, 5.0f);
    }
    break;

  case ENEMY_DIVER:
    if (diving)
    {
      // Home in on the paddle with a serpentine wiggle
      float targetX = paddle.pos.x - size / 2.0f;
      float dirX = clamp((targetX - enemy->pos.x) / 120.0f, -0.6f, 0.6f);
      enemy->vel.x = dirX + sinf(enemy->animTime * 6.0f) * 0.35f;
      enemy->vel.y = 1.0f;
      enemy->vel = vec2Norm(enemy->vel, 1);
      enemy->speed = 190;
    }
    break;

  case ENEMY_SHOOTER:
    if (enemy->actionTimer <= 0)
    {
      // Only shoot while visible on screen
      if (enemy->pos.y > camera.y && enemy->pos.y < camera.y + SCREEN_HEIGHT - 150)
      {
        spawnEnemyBullet(enemy->pos.x + size / 2.0f, enemy->pos.y + size);
      }
      enemy->actionTimer = frandRange(2.6f, 4.4f);
    }
    break;

  default:
    break;
  }

  // Wall bounces
  float nextX = enemy->pos.x + enemy->vel.x * enemy->speed * dt;
  float nextY = enemy->pos.y + enemy->vel.y * enemy->speed * dt;

  if (nextX < 0 || nextX + size > SCREEN_WIDTH)
  {
    enemy->vel.x *= -1;
  }

  if (nextY < camera.y && !diving)
  {
    enemy->vel.y = fabsf(enemy->vel.y);
  }

  if (!diving)
  {
    bounceOffBricks(enemy);
  }

  // Below the visible area: divers die quietly, others turn back up
  if (enemy->pos.y + size > camera.y + SCREEN_HEIGHT)
  {
    if (diving || enemy->type == ENEMY_MINI)
    {
      enemy->active = false;
      enemyDespawned(enemy);
      return;
    }

    enemy->vel.y = -fabsf(enemy->vel.y);
  }

  enemy->pos.x += enemy->vel.x * enemy->speed * dt;
  enemy->pos.y += enemy->vel.y * enemy->speed * dt;

  // Touching the paddle destroys the enemy
  if (checkCollision(
          enemy->pos.x, enemy->pos.y, size, size,
          paddle.pos.x - paddleWidth() / 2.0f, paddle.pos.y,
          paddleWidth(), PADDLE_HEIGHT))
  {
    killEnemy(enemy, 50);
  }
}

void drawEnemy(const Enemy *enemy)
{
  if (!enemy->active)
  {
    return;
  }

  int size = enemySize(enemy->type);
  float screenY = enemy->pos.y - camera.y;

  if (screenY < -size - 20 || screenY > SCREEN_HEIGHT + 20)
  {
    return;
  }

  SDL_Texture *tex = texEnemy[enemy->type];
  Uint8 alpha = (Uint8)(enemy->spawnFade * 255);

  float angle = 0;
  float bob = 0;
  SDL_Color halo;

  switch (enemy->type)
  {
  case ENEMY_DRIFTER:
  case ENEMY_MINI:
    angle = enemy->animTime * 80.0f;
    halo = (SDL_Color){255, 90, 210, 255};
    break;
  case ENEMY_DIVER:
    angle = sinf(enemy->animTime * 5.0f) * 12.0f;
    halo = (SDL_Color){255, 145, 60, 255};
    break;
  case ENEMY_SPLITTER:
    angle = sinf(enemy->animTime * 3.0f) * 8.0f;
    halo = (SDL_Color){140, 245, 90, 255};
    break;
  case ENEMY_SHOOTER:
    bob = sinf(enemy->animTime * 2.5f) * 3.0f;
    halo = (SDL_Color){120, 190, 255, 255};
    break;
  default:
    halo = (SDL_Color){255, 255, 255, 255};
    break;
  }

  // Pulsing halo behind the enemy
  float pulse = 0.75f + 0.25f * sinf(enemy->animTime * 4.0f);
  float haloSize = size * 2.0f * pulse;

  SDL_SetTextureColorMod(texGlow, halo.r, halo.g, halo.b);
  SDL_SetTextureAlphaMod(texGlow, (Uint8)(60 * pulse * enemy->spawnFade));
  SDL_FRect haloRect = {
      enemy->pos.x + size / 2.0f - haloSize / 2,
      screenY + bob + size / 2.0f - haloSize / 2,
      haloSize,
      haloSize};
  SDL_RenderTexture(renderer, texGlow, NULL, &haloRect);
  SDL_SetTextureColorMod(texGlow, 255, 255, 255);
  SDL_SetTextureAlphaMod(texGlow, 255);

  SDL_SetTextureAlphaMod(tex, alpha);
  SDL_FRect dst = {enemy->pos.x, screenY + bob, (float)size, (float)size};
  SDL_RenderTextureRotated(renderer, tex, NULL, &dst, angle, NULL, SDL_FLIP_NONE);
  SDL_SetTextureAlphaMod(tex, 255);
}
