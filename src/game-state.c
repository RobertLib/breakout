#include "game-state.h"
#include "game-over.h"
#include "intro-screen.h"
#include "playing.h"
#include "win-screen.h"

GameState nextGameState;

static GameState gameState;

void changeGameStateUpdate(void)
{
  if (gameState == nextGameState)
  {
    return;
  }

  destroyGameState();

  gameState = nextGameState;

  initializeGameState();
}

void initializeGameState(void)
{
  switch (gameState)
  {
  case GAME_STATE_INTRO_SCREEN:
    initializeIntroScreen();
    break;
  case GAME_STATE_PLAYING:
    initializePlaying();
    break;
  case GAME_STATE_GAME_OVER:
    initializeGameOver();
    break;
  case GAME_STATE_WIN_SCREEN:
    initializeWinScreen();
    break;
  }
}

void updateGameState(void)
{
  switch (gameState)
  {
  case GAME_STATE_INTRO_SCREEN:
    updateIntroScreen();
    break;
  case GAME_STATE_PLAYING:
    updatePlaying();
    break;
  case GAME_STATE_GAME_OVER:
    updateGameOver();
    break;
  case GAME_STATE_WIN_SCREEN:
    updateWinScreen();
    break;
  }
}

void drawGameState(void)
{
  switch (gameState)
  {
  case GAME_STATE_INTRO_SCREEN:
    drawIntroScreen();
    break;
  case GAME_STATE_PLAYING:
    drawPlaying();
    break;
  case GAME_STATE_GAME_OVER:
    drawGameOver();
    break;
  case GAME_STATE_WIN_SCREEN:
    drawWinScreen();
    break;
  }
}

void destroyGameState(void)
{
  switch (gameState)
  {
  case GAME_STATE_INTRO_SCREEN:
    destroyIntroScreen();
    break;
  case GAME_STATE_PLAYING:
    destroyPlaying();
    break;
  case GAME_STATE_GAME_OVER:
    destroyGameOver();
    break;
  case GAME_STATE_WIN_SCREEN:
    destroyWinScreen();
    break;
  }
}
