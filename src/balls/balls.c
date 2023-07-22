#include "balls.h"
#include "../lib/camera.h"
#include "../paddle/paddle.h"

Ball balls[BALL_COUNT];

SDL_Texture *ballTexture;

MIX_Audio *ballSound;

void activateAllBalls(void)
{
  for (int i = 0; i < BALL_COUNT; i++)
  {
    if (!balls[i].active)
    {
      initializeBall(&balls[i], true);
    }
  }
}

void resetBalls(void)
{
  for (int i = 0; i < BALL_COUNT; i++)
  {
    initializeBall(&balls[i], i == 0);
  }
}

bool isBallsDocked(void)
{
  bool allDocked = true;

  for (int i = 0; i < BALL_COUNT; i++)
  {
    if (!isBallDocked(&balls[i]))
    {
      allDocked = false;
      break;
    }
  }

  return allDocked;
}

void releaseBalls(void)
{
  for (int i = 0; i < BALL_COUNT; i++)
  {
    releaseBall(&balls[i]);
  }
}

void initializeBalls(void)
{
  resetBalls();

  ballTexture = loadTexture("src/balls/ball.bmp");
  ballSound = loadWAV("src/balls/ball.wav");
}

static bool isOffScreen(const Ball *ball)
{
  return ball->pos.x + BALL_SIZE < 0 ||
         ball->pos.x > SCREEN_WIDTH ||
         ball->pos.y > camera.y + SCREEN_HEIGHT;
}

void updateBalls(void)
{
  bool areAllActiveBallsOffScreen = true;

  for (int i = 0; i < BALL_COUNT; i++)
  {
    updateBall(&balls[i]);

    if (balls[i].active)
    {
      if (isOffScreen(&balls[i]))
      {
        balls[i].active = false;
      }
      else
      {
        areAllActiveBallsOffScreen = false;
      }
    }
  }

  if (areAllActiveBallsOffScreen)
  {
    paddle.type = PADDLE_TYPE_DYING;
  }
}

void drawBalls(void)
{
  for (int i = 0; i < BALL_COUNT; i++)
  {
    drawBall(&balls[i]);
  }
}

void destroyBalls(void)
{
  SDL_DestroyTexture(ballTexture);
  MIX_DestroyAudio(ballSound);

  ballTexture = NULL;
  ballSound = NULL;
}
