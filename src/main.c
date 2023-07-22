#include "bricks/brick.h"
#include "globals.h"
#include "lib/camera.h"
#include "lib/game-state.h"
#include "lib/transition.h"
#include "paddle/paddle.h"
#include "level-manager.h"
#include <time.h>

int main(void)
{
  srand((int)time(NULL));

  // Init SDL, SDL_ttf, SDL_mixer
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
  {
    fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  if (!TTF_Init())
  {
    fprintf(stderr, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", SDL_GetError());
    exit(1);
  }

  // Create window
  SDL_Window *window = SDL_CreateWindow(
      "Breakout",
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
      SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_FULLSCREEN);
#else
      0);
#endif

  if (window == NULL)
  {
    fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  // Create renderer
  renderer = SDL_CreateRenderer(window, NULL);

  if (renderer == NULL)
  {
    fprintf(stderr, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    exit(1);
  }

  // Enable vsync
  SDL_SetRenderVSync(renderer, 1);

  // Set logical size
  SDL_SetRenderLogicalPresentation(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

  // Initialize audio mixer
  if (!MIX_Init())
  {
    fprintf(stderr, "SDL_mixer could not initialize! SDL Error: %s\n", SDL_GetError());
    exit(1);
  }

  // Create audio mixer
  mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
  if (mixer == NULL)
  {
    fprintf(stderr, "SDL_mixer could not create mixer! SDL Error: %s\n", SDL_GetError());
    exit(1);
  }

  // Load fonts
  font16 = loadFont("src/assets/font.ttf", 16);
  font24 = loadFont("src/assets/font.ttf", 24);
  font32 = loadFont("src/assets/font.ttf", 32);

  // Hide cursor and enable relative mouse mode
  // SDL_ShowCursor(SDL_FALSE);
  // SDL_SetRelativeMouseMode(SDL_TRUE);

  // Initialization
  initializeLevelManager();
  initializeGameState();
  initializeTransition();
  initializeBrickProperties();

  bool fullscreen = false;
  bool running = true;

  Uint64 startTime = SDL_GetTicks();
  Uint64 lastTime = startTime;

  SDL_Event event;

  // Main loop
  while (running)
  {
    // Calculate delta time
    startTime = SDL_GetTicks();
    dt = (startTime - lastTime) / 1000.f;
    lastTime = startTime;

    // Input
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_EVENT_QUIT:
        running = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        isKeyDown[event.key.scancode] = true;

        switch (event.key.key)
        {
        case SDLK_ESCAPE:
        case SDLK_Q:
          running = false;
          break;
        case SDLK_F:
#if !defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE
          // Toggle fullscreen
          fullscreen = !fullscreen;

          SDL_SetWindowFullscreen(window, fullscreen);
#endif
          break;
        case SDLK_P:
          isPause = !isPause;
          break;
        }
        break;
      case SDL_EVENT_KEY_UP:
        isKeyDown[event.key.scancode] = false;
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        isMouseButtonDown[event.button.button] = true;
        break;
      case SDL_EVENT_MOUSE_BUTTON_UP:
        isMouseButtonDown[event.button.button] = false;
        break;
      case SDL_EVENT_MOUSE_MOTION:
        // Get relative motion
        motionX = (int)event.motion.x;
        motionY = (int)event.motion.y;
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
    if (1000.f / FPS > SDL_GetTicks() - startTime)
    {
      SDL_Delay((Uint32)(1000.f / FPS - (SDL_GetTicks() - startTime)));
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
  destroyLevelManager();

  TTF_CloseFont(font16);
  TTF_CloseFont(font24);
  TTF_CloseFont(font32);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  MIX_DestroyMixer(mixer);
  MIX_Quit();

  TTF_Quit();
  SDL_Quit();

  return 0;
}
