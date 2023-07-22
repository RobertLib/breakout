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
    SDL_RenderCopy(
        renderer,
        texture,
        NULL,
        &(SDL_Rect){
            SCREEN_WIDTH / 2 - getSize(texture).x / 2,
            SCREEN_HEIGHT / 2 - getSize(texture).y / 2 + yOffset,
            getSize(texture).x,
            getSize(texture).y});
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
