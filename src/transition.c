#include "transition.h"

static const int TEXTURE_SIZE = 8;

static float inProgress;
static float opacity;
static SDL_Texture *texture;

static void (*transitionCallback)(void);

static void transitionTimer(void)
{
  if (inProgress)
  {
    opacity += dt * 255 * 2;

    // If the transition is complete, call the callback
    if (opacity > 255 && transitionCallback != NULL)
    {
      transitionCallback();
      transitionCallback = NULL;
    }

    // If the transition is complete, reset the opacity and inProgress
    if (opacity > 255 * 2)
    {
      opacity = 0;

      inProgress = false;
    }
  }
}

void startTransition(void (*callback)(void))
{
  transitionCallback = callback;

  inProgress = true;
}

void initializeTransition(void)
{
  inProgress = false;
  opacity = 0;
  texture = loadTexture(assetsPath("transition.bmp"));

  transitionCallback = NULL;
}

void updateTransition(void)
{
  transitionTimer();
}

void drawTransition(void)
{
  SDL_SetTextureAlphaMod(texture, opacity > 255 ? 255 * 2 - opacity : opacity);

  SDL_RenderCopy(
      renderer,
      texture,
      &(SDL_Rect){0, 0, TEXTURE_SIZE, TEXTURE_SIZE},
      &(SDL_Rect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});

  SDL_SetTextureAlphaMod(texture, 255);
}

void destroyTransition(void)
{
  SDL_DestroyTexture(texture);

  texture = NULL;
}
