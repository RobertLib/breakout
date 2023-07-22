#include "intro-screen.h"
#include "../lib/game-state.h"

static SDL_Texture *logoText;
static SDL_Texture *infoText;

static void changeGameState(void)
{
  nextGameState = GAME_STATE_PLAYING_SCREEN;
}

void initializeIntroScreen(void)
{
  logoText = renderTextSolid(
      font32,
      "BREAKOUT",
      (SDL_Color){255, 255, 255, 255});

  infoText = renderTextSolid(
      font24,
      "Press SPACE to start a new game",
      (SDL_Color){255, 255, 255, 255});
}

void updateIntroScreen(void)
{
  if (isKeyDown[K_SPACE] || isMouseButtonDown[1])
  {
    startTransition(changeGameState);
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

void drawIntroScreen(void)
{
  renderCopyCenter(logoText, -40);
  renderCopyCenter(infoText, 40);
}

void destroyIntroScreen(void)
{
  SDL_DestroyTexture(logoText);
  SDL_DestroyTexture(infoText);

  logoText = NULL;
  infoText = NULL;
}
