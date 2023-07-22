#include "playing.h"
#include "balls.h"
#include "bricks.h"
#include "camera.h"
#include "enemies.h"
#include "paddle.h"
#include "status-bar.h"

float backgroundPosY;

static const int BACKGROUND_SIZE = 40;

static SDL_Texture *backgroundTexture;
static SDL_Texture *areYouReadyText;
static SDL_Texture *levelInfoText;
static SDL_Texture *goodWorkText;

void updatePlayingLevelInfoText(void)
{
  char levelInfoString[80];

  sprintf(levelInfoString, "Level: %d", paddle.level + 1);

  levelInfoText = renderTextSolid(
      font24,
      levelInfoString,
      (SDL_Color){255, 255, 255, 255});
}

void initializePlaying(void)
{
  initializePaddle();
  initializeBalls();
  initializeBricks();
  initializeEnemies();
  initializeCamera();
  initializeStatusBar();

  backgroundPosY = 0;

  backgroundTexture = loadTexture(assetsPath("background.bmp"));

  areYouReadyText = renderTextSolid(
      font32,
      "Are You Ready?",
      (SDL_Color){255, 255, 255, 255});

  goodWorkText = renderTextSolid(
      font32,
      "Good Work!",
      (SDL_Color){255, 255, 255, 255});

  updatePlayingLevelInfoText();
}

void updatePlaying(void)
{
  paddleTimerUpdate();

  if (isPause || paddle.levelCompleted)
  {
    return;
  }

  updatePaddle();
  updateBalls();
  updateBricks();
  updateEnemies();
  updateCamera();

  if (backgroundPosY >= BACKGROUND_SIZE)
  {
    backgroundPosY = 0;
  }
}

void drawPlaying(void)
{
  // Draw background
  for (int y = -BACKGROUND_SIZE; y < SCREEN_HEIGHT; y += BACKGROUND_SIZE)
  {
    for (int x = 0; x < SCREEN_WIDTH; x += BACKGROUND_SIZE)
    {
      SDL_RenderCopy(
          renderer,
          backgroundTexture,
          NULL,
          &(SDL_Rect){x, y + backgroundPosY, BACKGROUND_SIZE, BACKGROUND_SIZE});
    }
  }

  drawBalls();
  drawBricks();
  drawEnemies();
  drawPaddle();
  drawStatusBar();

  if (!paddle.isReady)
  {
    SDL_RenderCopy(
        renderer,
        areYouReadyText,
        NULL,
        &(SDL_Rect){
            SCREEN_WIDTH / 2 - getSize(areYouReadyText).x / 2,
            SCREEN_HEIGHT / 2 - getSize(areYouReadyText).y / 2,
            getSize(areYouReadyText).x,
            getSize(areYouReadyText).y});

    SDL_RenderCopy(
        renderer,
        levelInfoText,
        NULL,
        &(SDL_Rect){
            SCREEN_WIDTH / 2 - getSize(levelInfoText).x / 2,
            SCREEN_HEIGHT / 2 - getSize(levelInfoText).y / 2 + 60,
            getSize(levelInfoText).x,
            getSize(levelInfoText).y});
  }

  if (paddle.levelCompleted)
  {
    SDL_RenderCopy(
        renderer,
        goodWorkText,
        NULL,
        &(SDL_Rect){
            SCREEN_WIDTH / 2 - getSize(goodWorkText).x / 2,
            SCREEN_HEIGHT / 2 - getSize(goodWorkText).y / 2,
            getSize(goodWorkText).x,
            getSize(goodWorkText).y});
  }
}

void destroyPlaying(void)
{
  destroyPaddle();
  destroyBalls();
  destroyBricks();
  destroyEnemies();
  destroyStatusBar();

  SDL_DestroyTexture(backgroundTexture);
  SDL_DestroyTexture(areYouReadyText);
  SDL_DestroyTexture(levelInfoText);
  SDL_DestroyTexture(goodWorkText);

  backgroundTexture = NULL;
  areYouReadyText = NULL;
  levelInfoText = NULL;
  goodWorkText = NULL;
}
