#include "win-screen.h"
#include "../lib/game-state.h"
#include "../paddle/paddle.h"

static SDL_Texture *headerText;
static SDL_Texture *infoText;
static SDL_Texture *scoreText;

void initializeWinScreen(void)
{
  headerText = renderTextSolid(
      font32,
      "YOU WIN!",
      (SDL_Color){255, 255, 255, 255});

  infoText = renderTextSolid(
      font24,
      "Press SPACE to start a new game",
      (SDL_Color){255, 255, 255, 255});

  char scoreString[80];

  sprintf(scoreString, "Your score is: %d", paddle.score);

  scoreText = renderTextSolid(
      font24,
      scoreString,
      (SDL_Color){255, 255, 255, 255});
}

void updateWinScreen(void)
{
  if (isKeyDown[K_SPACE] || isMouseButtonDown[1])
  {
    nextGameState = GAME_STATE_PLAYING_SCREEN;
  }
}

static void renderCopyCenter(SDL_Texture *texture, const int yOffset)
{
  if (texture != NULL)
  {
    SDL_FPoint size = getSize(texture);
    SDL_FRect dstRect = {
        SCREEN_WIDTH / 2 - size.x / 2,
        SCREEN_HEIGHT / 2 - size.y / 2 + yOffset,
        size.x,
        size.y};
    SDL_RenderTexture(renderer, texture, NULL, &dstRect);
  }
}

void drawWinScreen(void)
{
  renderCopyCenter(headerText, -60);
  renderCopyCenter(scoreText, 0);
  renderCopyCenter(infoText, 60);
}

void destroyWinScreen(void)
{
  SDL_DestroyTexture(headerText);
  SDL_DestroyTexture(infoText);
  SDL_DestroyTexture(scoreText);

  headerText = NULL;
  infoText = NULL;
  scoreText = NULL;
}
