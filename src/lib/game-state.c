#include "game-state.h"
#include "../screens/game-over-screen.h"
#include "../screens/intro-screen.h"
#include "../screens/playing-screen.h"
#include "../screens/win-screen.h"
#ifndef NDEBUG
#include "../screens/editor-screen.h"
#endif

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
  case GAME_STATE_PLAYING_SCREEN:
    initializePlaying();
    break;
  case GAME_STATE_GAME_OVER_SCREEN:
    initializeGameOverScreen();
    break;
  case GAME_STATE_WIN_SCREEN:
    initializeWinScreen();
    break;
#ifndef NDEBUG
  case GAME_STATE_EDITOR_SCREEN:
    initializeEditorScreen();
    break;
#endif
  }
}

void updateGameState(void)
{
  switch (gameState)
  {
  case GAME_STATE_INTRO_SCREEN:
    updateIntroScreen();
    break;
  case GAME_STATE_PLAYING_SCREEN:
    updatePlaying();
    break;
  case GAME_STATE_GAME_OVER_SCREEN:
    updateGameOverScreen();
    break;
  case GAME_STATE_WIN_SCREEN:
    updateWinScreen();
    break;
#ifndef NDEBUG
  case GAME_STATE_EDITOR_SCREEN:
    updateEditorScreen();
    break;
#endif
  }
}

void drawGameState(void)
{
  switch (gameState)
  {
  case GAME_STATE_INTRO_SCREEN:
    drawIntroScreen();
    break;
  case GAME_STATE_PLAYING_SCREEN:
    drawPlaying();
    break;
  case GAME_STATE_GAME_OVER_SCREEN:
    drawGameOverScreen();
    break;
  case GAME_STATE_WIN_SCREEN:
    drawWinScreen();
    break;
#ifndef NDEBUG
  case GAME_STATE_EDITOR_SCREEN:
    drawEditorScreen();
    break;
#endif
  }
}

void destroyGameState(void)
{
  switch (gameState)
  {
  case GAME_STATE_INTRO_SCREEN:
    destroyIntroScreen();
    break;
  case GAME_STATE_PLAYING_SCREEN:
    destroyPlaying();
    break;
  case GAME_STATE_GAME_OVER_SCREEN:
    destroyGameOverScreen();
    break;
  case GAME_STATE_WIN_SCREEN:
    destroyWinScreen();
    break;
#ifndef NDEBUG
  case GAME_STATE_EDITOR_SCREEN:
    destroyEditorScreen();
    break;
#endif
  }
}
