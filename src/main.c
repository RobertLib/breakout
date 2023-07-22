#include "brick.h"
#include "camera.h"
#include "game-state.h"
#include "globals.h"
#include "paddle.h"
#include "transition.h"
#include <time.h>

int main(int argc, char *argv[])
{
  srand((int)time(NULL));

  // Init SDL, SDL_ttf, SDL_mixer
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  if (TTF_Init() == -1)
  {
    fprintf(stderr, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    exit(1);
  }

  if (Mix_OpenAudio(44000, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
  {
    fprintf(stderr, "SDL_mixer could not open! SDL_mixer Error: %s\n", Mix_GetError());
    exit(1);
  }

  // Create window
  SDL_Window *window = SDL_CreateWindow(
      "Breakout",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
      SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
      0);
#endif

  if (window == NULL)
  {
    fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  // Create renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == NULL)
  {
    fprintf(stderr, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    exit(1);
  }

  // Set logical size
  SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

  // Load fonts
  font24 = TTF_OpenFont(assetsPath("font.ttf"), 24);
  font32 = TTF_OpenFont(assetsPath("font.ttf"), 32);

  if (font24 == NULL || font32 == NULL)
  {
    fprintf(stderr, "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
    exit(1);
  }

  // Hide cursor and enable relative mouse mode
  // SDL_ShowCursor(SDL_FALSE);
  // SDL_SetRelativeMouseMode(SDL_TRUE);

  // Initialization
  initializeGameState();
  initializeTransition();
  initializeBrickProperties();

  bool fullscreen = false;
  bool running = true;

  Uint64 startTime = SDL_GetTicks64();
  Uint64 lastTime = startTime;

  SDL_Event event;

  // Main loop
  while (running)
  {
    // Calculate delta time
    startTime = SDL_GetTicks64();
    dt = (startTime - lastTime) / 1000.f;
    lastTime = startTime;

    // Input
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        isKeyDown[event.key.keysym.scancode] = true;

        switch (event.key.keysym.sym)
        {
        case SDLK_ESCAPE:
        case SDLK_q:
          running = false;
          break;
        case SDLK_f:
#if !defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE
          // Toggle fullscreen
          fullscreen = !fullscreen;

          SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
#endif
          break;
        case SDLK_p:
          isPause = !isPause;
          break;
        }
        break;
      case SDL_KEYUP:
        isKeyDown[event.key.keysym.scancode] = false;
        break;
      case SDL_MOUSEBUTTONDOWN:
        isMouseButtonDown[event.button.button] = true;
        break;
      case SDL_MOUSEBUTTONUP:
        isMouseButtonDown[event.button.button] = false;
        break;
      case SDL_MOUSEMOTION:
        // Get relative motion
        motionX = event.motion.x;
        motionY = event.motion.y;
        break;
      }
    }

    // Update
    updateGameState();
    updateTransition();

    // Draw
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);

    SDL_RenderClear(renderer);

    drawGameState();
    drawTransition();

    SDL_RenderPresent(renderer);

    // Cap the frame rate
    if (1000.f / FPS > SDL_GetTicks64() - startTime)
    {
      SDL_Delay(1000.f / FPS - (SDL_GetTicks64() - startTime));
    }

    // Update previous motion
    prevMotionX = motionX;
    prevMotionY = motionY;

    // Update previous camera position
    camera.prevY = camera.y;

    paddleNextLevelUpdate();
    changeGameStateUpdate();
  }

  // Cleanup
  destroyGameState();
  destroyTransition();

  TTF_CloseFont(font24);
  TTF_CloseFont(font32);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  Mix_CloseAudio();

  TTF_Quit();
  SDL_Quit();

  return 0;
}
