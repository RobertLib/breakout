#include "playing-screen.h"
#include "../balls/balls.h"
#include "../bricks/bricks.h"
#include "../enemies/enemies.h"
#include "../lib/camera.h"
#include "../lib/game-state.h"
#include "../paddle/paddle.h"
#include "../ui/status-bar.h"
#ifndef NDEBUG
#include "editor-screen.h"
#endif

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

  backgroundTexture = loadTexture("src/screens/background.bmp");

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
#ifndef NDEBUG
  // Handle E to open editor with current level
  if (isKeyDown[SDL_SCANCODE_E] && !ePressed)
  {
    ePressed = true;
    // Set editor level BEFORE state change
    setEditorLevel(paddle.level);
    nextGameState = GAME_STATE_EDITOR_SCREEN;
    return;
  }
  else if (!isKeyDown[SDL_SCANCODE_E])
  {
    ePressed = false;
  }
#endif

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
      SDL_FRect dstRect = {(float)x, (float)(y + backgroundPosY), (float)BACKGROUND_SIZE, (float)BACKGROUND_SIZE};
      SDL_RenderTexture(renderer, backgroundTexture, NULL, &dstRect);
    }
  }

  drawBalls();
  drawBricks();
  drawEnemies();
  drawPaddle();
  drawStatusBar();

  if (!paddle.isReady)
  {
    SDL_FPoint readySize = getSize(areYouReadyText);
    SDL_FRect readyRect = {
        SCREEN_WIDTH / 2 - readySize.x / 2,
        SCREEN_HEIGHT / 2 - readySize.y / 2,
        readySize.x,
        readySize.y};
    SDL_RenderTexture(renderer, areYouReadyText, NULL, &readyRect);

    SDL_FPoint levelSize = getSize(levelInfoText);
    SDL_FRect levelRect = {
        SCREEN_WIDTH / 2 - levelSize.x / 2,
        SCREEN_HEIGHT / 2 - levelSize.y / 2 + 60,
        levelSize.x,
        levelSize.y};
    SDL_RenderTexture(renderer, levelInfoText, NULL, &levelRect);
  }

  if (paddle.levelCompleted)
  {
    SDL_FPoint goodSize = getSize(goodWorkText);
    SDL_FRect goodRect = {
        SCREEN_WIDTH / 2 - goodSize.x / 2,
        SCREEN_HEIGHT / 2 - goodSize.y / 2,
        goodSize.x,
        goodSize.y};
    SDL_RenderTexture(renderer, goodWorkText, NULL, &goodRect);
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
