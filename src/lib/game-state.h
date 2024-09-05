#pragma once

typedef enum GameState
{
  GAME_STATE_INTRO_SCREEN,
  GAME_STATE_PLAYING_SCREEN,
  GAME_STATE_GAME_OVER_SCREEN,
  GAME_STATE_WIN_SCREEN
} GameState;

extern GameState nextGameState;

void changeGameStateUpdate(void);

void initializeGameState(void);

void updateGameState(void);

void drawGameState(void);

void destroyGameState(void);
