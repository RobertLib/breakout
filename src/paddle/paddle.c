#include "paddle.h"
#include "../balls/balls.h"
#include "../bricks/bricks.h"
#include "../enemies/enemies.h"
#include "../level-types.h"
#include "../level-manager.h"
#include "../lib/camera.h"
#include "../lib/game-state.h"
#include "../screens/playing-screen.h"
#include "../ui/status-bar.h"

Paddle paddle;

static int requestedStartLevel = -1;

static const int DEFAULT_WIDTH = 100;
static const int LONG_WIDTH = 150;
static const int SPEED = 500;
static const int LIVES = 3;

static const float FRICTION = 10;

static void levelCompletedCallback(Obj *obj);
static void lifeLossCallback(Obj *obj);
static void readyCallback(Obj *obj);
static void returnTypeToDefaultCallback(Obj *obj);
static void shotCallback(Obj *obj);
static void timeAfterStartCallback(Obj *obj);

static Timer levelCompletedTimer = {0, 1, &levelCompletedCallback, NULL};
static Timer lifeLossTimer = {0, 1, &lifeLossCallback, NULL};
static Timer readyTimer = {0, 2, &readyCallback, NULL};
static Timer returnTypeToDefaultTimer = {0, 5, &returnTypeToDefaultCallback, NULL};
static Timer shotTimer = {0, 0.5, &shotCallback, NULL};
static Timer timeAfterStartTimer = {0, 3, &timeAfterStartCallback, NULL};

static Bullet *findInactiveBullet(void)
{
  for (int i = 0; i < NUMBER_OF_BULLETS; i++)
  {
    if (!paddle.bullets[i].active)
    {
      return &paddle.bullets[i];
    }
  }

  return NULL;
}

// Reset paddle to initial state
static void reset(void)
{
  paddle.pos.x = SCREEN_WIDTH / 2;
  paddle.vel.x = 0;
  paddle.vel.y = 0;
  paddle.moveTo.x = -1;
  paddle.moveTo.y = -1;
  paddle.isReady = false;
  paddle.isTimeAfterStart = false;
  paddle.type = PADDLE_TYPE_DEFAULT;
  paddle.canShoot = true;
  paddle.inCollision = false;
  paddle.levelCompleted = false;
}

static void respawnOrGameOver(void)
{
  if (paddle.lives > 1)
  {
    reset();

    paddle.lives--;

    updateLivesInStatusBar();

    resetBalls();
  }
  else
  {
    nextGameState = GAME_STATE_GAME_OVER_SCREEN;
  }
}

static void shoot(void)
{
  if (paddle.type != PADDLE_TYPE_SHOOTING || !paddle.canShoot)
  {
    return;
  }

  Bullet *bullet = findInactiveBullet();

  bullet->pos.x = paddle.pos.x - BULLET_WIDTH / 2;
  bullet->pos.y = paddle.pos.y;
  bullet->active = true;

  paddle.canShoot = false;
}

void addScore(const int value)
{
  paddle.score += value;
}

void setStartLevel(int level)
{
  requestedStartLevel = level;
}

void changePaddleType(PaddleType type)
{
  if (paddle.type == type)
  {
    return;
  }

  switch (type)
  {
  case PADDLE_TYPE_ADD_LIFE:
    paddle.lives++;
    updateLivesInStatusBar();
    break;
  case PADDLE_TYPE_MORE_BALLS:
    activateAllBalls();
    break;
  default:
    break;
  }

  paddle.type = type;
}

void paddleNextLevelUpdate(void)
{
  if (!paddle.nextLevel)
  {
    return;
  }

  // If the player has completed all levels
  if (paddle.level < getNumberOfLevels() - 1)
  {
    reset();

    // Move the paddle to the bottom of the screen
    paddle.pos.y = LEVEL_HEIGHT - PADDLE_HEIGHT - 30;
    // Reset the background position
    backgroundPosY = 0;

    resetBalls();

    destroyBricks();
    destroyEnemies();

    // Increase the level
    paddle.level++;

    initializeBricks();
    initializeEnemies();
    initializeCamera();
    initializeStatusBar();

    updatePlayingLevelInfoText();
  }
  else
  {
    nextGameState = GAME_STATE_WIN_SCREEN;
  }

  paddle.nextLevel = false;
}

void paddleTimerUpdate(void)
{
  if (paddle.levelCompleted)
  {
    updateTimer(&levelCompletedTimer);
  }
}

int paddleWidth(void)
{
  switch (paddle.type)
  {
  case PADDLE_TYPE_LONG:
    return LONG_WIDTH;
  default:
    return DEFAULT_WIDTH;
  }
}

void initializePaddle(void)
{
  reset();

  paddle.pos.y = LEVEL_HEIGHT - PADDLE_HEIGHT - 30;
  paddle.score = 0;
  paddle.lives = LIVES;

  // Use requested level if set, otherwise start at 0
  if (requestedStartLevel >= 0)
  {
    paddle.level = requestedStartLevel;
    requestedStartLevel = -1; // Reset for next time
  }
  else
  {
    paddle.level = 0;
  }

  paddle.nextLevel = false;

  // Initialize bullets
  for (int i = 0; i < NUMBER_OF_BULLETS; i++)
  {
    initializeBullet(&paddle.bullets[i]);
  }

  paddle.texture = loadTexture("src/paddle/paddle.bmp");
}

static void updatePosition(void)
{
  if (isKeyDown[K_LEFT])
  {
    paddle.vel.x = -SPEED;
  }
  if (isKeyDown[K_RIGHT])
  {
    paddle.vel.x = SPEED;
  }

  // Move paddle with mouse
  if (isMouseButtonDown[1])
  {
    paddle.moveTo.x = motionX;
  }

  // Move paddle to the position of the mouse
  if (paddle.pos.x == paddle.moveTo.x)
  {
    paddle.moveTo.x = -1;
  }
  else if (paddle.moveTo.x != -1)
  {
    paddle.pos.x = lerp(paddle.pos.x, paddle.moveTo.x, 0.5);
  }

  // Update paddle position
  paddle.pos.x += paddle.vel.x * dt;
  // Apply friction
  paddle.vel.x /= 1 + FRICTION * dt;

  // Prevent paddle from going out of bounds
  int halfWidth = paddleWidth() / 2;

  if (paddle.pos.x - halfWidth < 0)
  {
    paddle.pos.x = halfWidth;
  }
  else if (paddle.pos.x + halfWidth > SCREEN_WIDTH)
  {
    paddle.pos.x = SCREEN_WIDTH - halfWidth;
  }
}

static void updateTimers(void)
{
  if (paddle.type == PADDLE_TYPE_DYING)
  {
    updateTimer(&lifeLossTimer);
  }
  else
  {
    lifeLossTimer.elapsedTime = 0;
  }

  if (!paddle.isReady)
  {
    updateTimer(&readyTimer);
  }

  if (paddle.type != PADDLE_TYPE_DEFAULT)
  {
    updateTimer(&returnTypeToDefaultTimer);
  }
  else
  {
    returnTypeToDefaultTimer.elapsedTime = 0;
  }

  if (!paddle.canShoot)
  {
    updateTimer(&shotTimer);
  }

  if (paddle.isTimeAfterStart)
  {
    updateTimer(&timeAfterStartTimer);
  }
}

static void handleShooting(void)
{
  if (isKeyDown[K_SPACE] || isMouseButtonDown[1])
  {
    shoot();
  }
}

static void updateBullets(void)
{
  for (int i = 0; i < NUMBER_OF_BULLETS; i++)
  {
    updateBullet(&paddle.bullets[i]);
  }
}

static void checkPaddleCollision(void)
{
  paddle.inCollision = false;

  for (int i = 0; i < numBricks; i++)
  {
    if (checkCollision(
            paddle.pos.x - paddleWidth() / 2,
            paddle.pos.y,
            paddleWidth(),
            PADDLE_HEIGHT,
            bricks[i].pos.x,
            bricks[i].pos.y,
            BRICK_WIDTH,
            BRICK_HEIGHT) &&
        bricks[i].active)
    {
      paddle.inCollision = true;
      break;
    }
  }
}

void updatePaddle(void)
{
  updatePosition();
  updateTimers();
  handleShooting();
  updateBullets();
  checkPaddleCollision();
}

void drawPaddle(void)
{
  if (paddle.type == PADDLE_TYPE_DYING)
  {
    SDL_SetTextureAlphaMod(paddle.texture, 255 - lifeLossTimer.elapsedTime * 255);
  }
  else
  {
    SDL_SetTextureAlphaMod(paddle.texture, 255);
  }

  if (paddle.type == PADDLE_TYPE_SHOOTING)
  {
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

    SDL_FRect shootRect = {
        paddle.pos.x - BULLET_WIDTH / 2,
        paddle.pos.y - 5 - camera.y,
        BULLET_WIDTH,
        5};
    SDL_RenderFillRect(renderer, &shootRect);
  }

  SDL_FRect srcRect = {(float)paddle.type, 0, 1, PADDLE_HEIGHT};
  SDL_FRect dstRect = {
      paddle.pos.x - paddleWidth() / 2,
      paddle.pos.y - camera.y,
      (float)paddleWidth(),
      PADDLE_HEIGHT};
  SDL_RenderTexture(renderer, paddle.texture, &srcRect, &dstRect);

  SDL_SetTextureAlphaMod(paddle.texture, 255);

  for (int i = 0; i < NUMBER_OF_BULLETS; i++)
  {
    drawBullet(&paddle.bullets[i]);
  }
}

void destroyPaddle(void)
{
  SDL_DestroyTexture(paddle.texture);

  paddle.texture = NULL;
}

// Callbacks

static void levelCompletedCallback(UNUSED Obj *obj)
{
  paddle.levelCompleted = false;
  paddle.nextLevel = true;
}

static void lifeLossCallback(UNUSED Obj *obj)
{
  paddle.type = PADDLE_TYPE_DEFAULT;

  respawnOrGameOver();
}

static void readyCallback(UNUSED Obj *obj)
{
  paddle.isReady = true;
  paddle.isTimeAfterStart = true;
}

static void returnTypeToDefaultCallback(UNUSED Obj *obj)
{
  changePaddleType(PADDLE_TYPE_DEFAULT);
}

static void shotCallback(UNUSED Obj *obj)
{
  paddle.canShoot = true;
}

static void timeAfterStartCallback(UNUSED Obj *obj)
{
  paddle.isTimeAfterStart = false;

  if (isBallsDocked())
  {
    releaseBalls();
  }
}
