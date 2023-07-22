#include "globals.h"
#include "lib/audio.h"
#include "lib/camera.h"
#include "lib/game-state.h"
#include "lib/gfx.h"
#include "lib/save.h"
#include "lib/starfield.h"
#include "lib/transition.h"
#include "paddle/paddle.h"
#include "ui/floating-text.h"
#include "level-manager.h"
#include "version.h"
#include <time.h>

// The filename for one frame of a capture: the path as given when there is only
// one frame, and `path-0000.bmp` for a burst, so that a burst assembles in
// filename order whatever puts it together.
static void shotFilename(char *out, size_t size, const char *base, int index,
                         int total)
{
  if (total <= 1)
  {
    SDL_strlcpy(out, base, size);
    return;
  }

  const char *dot = SDL_strrchr(base, '.');
  int stem = dot != NULL ? (int)(dot - base) : (int)SDL_strlen(base);

  snprintf(out, size, "%.*s-%04d%s", stem, base, index,
           dot != NULL ? dot : ".bmp");
}

// One frame, written as a BMP, cropped to the game's own frame.
//
// SDL_RenderReadPixels reads the pixels on the screen, and under letterbox
// presentation those are not the pixels the game drew into: it is the whole
// window, black bars included, at whatever scale that window imposes on the
// logical 800x600. This read back 1024x768 on a machine whose save had
// `fullscreen=1` in it, so every screenshot ever taken here arrived as pixel art
// scaled by 1.28 before anything downstream had touched it.
// SDL_GetRenderLogicalPresentationRect is where the content actually landed -
// 800x600 exactly in a window that size, a whole multiple of it on a
// high-density display, and no bar in either case.
static bool saveShot(const char *path, int frame)
{
  SDL_FRect area;
  SDL_Rect crop;
  const SDL_Rect *rect = NULL;

  if (SDL_GetRenderLogicalPresentationRect(renderer, &area))
  {
    crop.x = (int)SDL_lroundf(area.x);
    crop.y = (int)SDL_lroundf(area.y);
    crop.w = (int)SDL_lroundf(area.w);
    crop.h = (int)SDL_lroundf(area.h);
    rect = &crop;
  }

  SDL_Surface *shot = SDL_RenderReadPixels(renderer, rect);

  if (shot == NULL)
  {
    fprintf(stderr, "Could not read frame %d: %s\n", frame, SDL_GetError());
    return false;
  }

  bool written = SDL_SaveBMP(shot, path);

  if (!written)
  {
    fprintf(stderr, "Could not write %s: %s\n", path, SDL_GetError());
  }
  else
  {
    // One line per file, and it names the size, because a capture whose frame
    // came out at the wrong size is the failure this whole path is about.
    printf("Wrote %s (%dx%d) at frame %d\n", path, shot->w, shot->h, frame);
  }

  SDL_DestroySurface(shot);

  return written;
}

int main(void)
{
  // Development helpers, all read here at the top because BREAKUP_SHOT decides
  // how the rest of this function behaves and everything it changes is set up
  // before the main loop:
  //
  //   BREAKUP_STATE=intro|menu|levels|playing|gameover|win  jump to a state
  //   BREAKUP_LEVEL=N   start playing at level N (1-based)
  //   BREAKUP_SCORE=N   start with N points
  //   BREAKUP_UNLOCKED=N   pretend N levels have been unlocked
  //   BREAKUP_KEYS=frame:scancode,frame:scancode  inject key presses
  //   BREAKUP_AUTOPLAY=1   the paddle tracks the ball (see playing-screen.c)
  //   BREAKUP_SEED=N    seed the RNG, instead of the clock
  //   BREAKUP_SHOT=frame:path.bmp   save a screenshot at that frame and quit
  //   BREAKUP_SHOT_FRAMES=N   write N frames from there on, path-0000.bmp up
  //   BREAKUP_SHOT_STEP=K     keep every K-th frame of that burst
  const char *stateEnv = SDL_getenv("BREAKUP_STATE");
  const char *levelEnv = SDL_getenv("BREAKUP_LEVEL");
  const char *scoreEnv = SDL_getenv("BREAKUP_SCORE");
  const char *unlockedEnv = SDL_getenv("BREAKUP_UNLOCKED");
  const char *keysEnv = SDL_getenv("BREAKUP_KEYS");
  const char *seedEnv = SDL_getenv("BREAKUP_SEED");
  const char *shotEnv = SDL_getenv("BREAKUP_SHOT");
  const char *shotFramesEnv = SDL_getenv("BREAKUP_SHOT_FRAMES");
  const char *shotStepEnv = SDL_getenv("BREAKUP_SHOT_STEP");

  // Seeded on request, so that two captures of one commit are two copies of the
  // same picture: the starfields, the particles, the power-up drops and the
  // enemies all come out of this. The clock stays the default - a player wants a
  // different night every time, and only a measurement wants the same one twice.
  srand(seedEnv != NULL ? (unsigned int)SDL_atoi(seedEnv)
                        : (unsigned int)time(NULL));

  int shotFrame = -1;
  int shotFrames = 1;
  int shotStep = 1;
  char shotPath[512] = {0};

  if (shotEnv != NULL)
  {
    const char *colon = SDL_strchr(shotEnv, ':');

    if (colon != NULL)
    {
      shotFrame = SDL_atoi(shotEnv);
      SDL_strlcpy(shotPath, colon + 1, sizeof(shotPath));
    }
    else
    {
      fprintf(stderr, "Could not read BREAKUP_SHOT=%s; it wants frame:path.bmp\n",
              shotEnv);
    }
  }

  // A capture is a *scripted* run, and that is three things beyond writing a
  // file. It reads and writes no save, so it plays the game as it ships rather
  // than as this machine has it - which matters most for the fullscreen flag,
  // because that is what decides the size every captured frame comes out at, and
  // it means a press run cannot overwrite anybody's progress or high scores. It
  // advances the world by a fixed step rather than by the wall clock, so a frame
  // number is a moment in the game instead of a moment on the machine. And it
  // paces itself to nothing at all - neither the frame cap at the bottom of the
  // loop nor vsync - so a shot deep into a level costs a fraction of the time
  // that level takes to play.
  const bool scripted = shotFrame >= 0;

  if (scripted)
  {
    if (shotFramesEnv != NULL)
    {
      shotFrames = SDL_max(1, SDL_atoi(shotFramesEnv));
    }

    if (shotStepEnv != NULL)
    {
      shotStep = SDL_max(1, SDL_atoi(shotStepEnv));
    }
  }

  setSaveScripted(scripted);

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
  // High pixel density keeps rendering sharp on Retina/HiDPI displays
  window = SDL_CreateWindow(
      BREAKUP_APP_NAME,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
      SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_FULLSCREEN);
#else
      SDL_WINDOW_HIGH_PIXEL_DENSITY);
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

  // Enable vsync - except in a scripted run, which is stepping the world by a
  // fixed amount and writing the frames to a disk, and has nothing to gain by
  // waiting for a display nobody is looking at. This, rather than the frame cap
  // at the bottom of the loop, is what was pacing a capture: with the cap
  // skipped and vsync left on, twenty seconds of game still cost twenty seconds
  // of waiting, because the wait had simply moved into SDL_RenderPresent.
  SDL_SetRenderVSync(renderer, scripted ? 0 : 1);

  // Set logical size
  SDL_SetRenderLogicalPresentation(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

  // Initialize audio mixer (the game keeps running without audio if this fails)
  if (MIX_Init())
  {
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

    if (mixer == NULL)
    {
      fprintf(stderr, "SDL_mixer could not create mixer! SDL Error: %s\n", SDL_GetError());
    }
  }
  else
  {
    fprintf(stderr, "SDL_mixer could not initialize! SDL Error: %s\n", SDL_GetError());
  }

  // Load fonts
  font16 = loadFont("assets/font.ttf", 16);
  font24 = loadFont("assets/font.ttf", 24);
  font32 = loadFont("assets/font.ttf", 32);
  font48 = loadFont("assets/font.ttf", 48);
  font64 = loadFont("assets/font.ttf", 64);

  // Initialization
  loadSave();
  initializeLevelManager();
  initializeGfx();
  initializeAudio();
  initializeStarfield();
  initializeGameState();
  initializeTransition();

  setMusicVolume(saveData.musicVol);
  setSfxVolume(saveData.sfxVol);

  if (saveData.fullscreen)
  {
    SDL_SetWindowFullscreen(window, true);
  }

  // Not addScore(), which is what this was: initializePlaying() runs later, at
  // the bottom of the first frame, and initializePaddle() sets the score to
  // zero on its way past - so BREAKUP_SCORE was silently ignored by every
  // capture of the one screen that shows a score being played for.
  // setStartScore() both sets it now (for =gameover and =win, which never
  // initialize a paddle) and survives the initializePaddle() that follows.
  if (scoreEnv != NULL)
  {
    setStartScore(SDL_atoi(scoreEnv));
  }

  // Only the level select screen reads this, and it is the one screen a capture
  // cannot photograph honestly without it: a scripted run starts from the
  // shipped save, where exactly one level is unlocked, so the grid is nineteen
  // padlocks and no level names. It is not written to any save file.
  //
  // Both of the clamps below go the wrong way round when there are no levels
  // at all: clamp(n, 1, 0) is 0 and clamp(n, 0, -1) is -1, and -1 is what
  // setStartLevel() would then hand to the bricks. A build whose assets did
  // not embed is a broken build, but it should say so rather than index
  // backwards off an array.
  const bool haveLevels = getNumberOfLevels() > 0;

  if (!haveLevels && (unlockedEnv != NULL || levelEnv != NULL))
  {
    fprintf(stderr, "No levels loaded; BREAKUP_LEVEL and BREAKUP_UNLOCKED ignored\n");
  }

  if (haveLevels && unlockedEnv != NULL)
  {
    saveData.unlocked = clamp(SDL_atoi(unlockedEnv), 1, getNumberOfLevels());
  }

  if (haveLevels && levelEnv != NULL)
  {
    setStartLevel(clamp(SDL_atoi(levelEnv) - 1, 0, getNumberOfLevels() - 1));
    nextGameState = GAME_STATE_PLAYING_SCREEN;
  }
  else if (stateEnv != NULL)
  {
    if (SDL_strcmp(stateEnv, "intro") == 0)
      nextGameState = GAME_STATE_INTRO_SCREEN;
    else if (SDL_strcmp(stateEnv, "menu") == 0)
      nextGameState = GAME_STATE_MENU_SCREEN;
    else if (SDL_strcmp(stateEnv, "levels") == 0)
      nextGameState = GAME_STATE_LEVEL_SELECT_SCREEN;
    else if (SDL_strcmp(stateEnv, "playing") == 0)
      nextGameState = GAME_STATE_PLAYING_SCREEN;
    else if (SDL_strcmp(stateEnv, "gameover") == 0)
      nextGameState = GAME_STATE_GAME_OVER_SCREEN;
    else if (SDL_strcmp(stateEnv, "win") == 0)
      nextGameState = GAME_STATE_WIN_SCREEN;
  }

  int keyFrames[32];
  int keyCodes[32];
  int numKeyEvents = 0;

  if (keysEnv != NULL)
  {
    const char *p = keysEnv;
    while (*p != '\0' && numKeyEvents < 32)
    {
      keyFrames[numKeyEvents] = SDL_atoi(p);
      const char *colon = SDL_strchr(p, ':');
      if (colon == NULL)
      {
        break;
      }
      int scancode = SDL_atoi(colon + 1);

      // isKeyPressed is an array of SDL_SCANCODE_COUNT bools and this number
      // comes off the command line, so it is checked here rather than trusted
      // at the point it is used to index one.
      if (scancode < 0 || scancode >= SDL_SCANCODE_COUNT)
      {
        fprintf(stderr, "BREAKUP_KEYS: %d is not a scancode (0..%d); ignored\n",
                scancode, SDL_SCANCODE_COUNT - 1);
      }
      else
      {
        keyCodes[numKeyEvents] = scancode;
        numKeyEvents++;
      }

      const char *comma = SDL_strchr(p, ',');
      if (comma == NULL)
      {
        break;
      }
      p = comma + 1;
    }
  }

  int frameCounter = 0;
  int shotsTaken = 0;

  bool running = true;

  Uint64 startTime = SDL_GetTicks();
  Uint64 lastTime = startTime;

  SDL_Event event;

  // Main loop
  while (running && !quitRequested)
  {
    // Calculate delta time (clamped to avoid tunneling after hitches)
    startTime = SDL_GetTicks();
    dt = (startTime - lastTime) / 1000.f;
    lastTime = startTime;

    if (dt > 1.0 / 30.0)
    {
      dt = 1.0 / 30.0;
    }

    // A scripted run steps the world instead of watching the clock, so that the
    // same frame number is the same moment in the game on any machine and at any
    // speed. Without it a capture is a photograph of how fast the machine that
    // took it happened to be.
    if (scripted)
    {
      dt = 1.0 / FPS;
    }

    gameTime += (float)dt;

    // Input
    while (SDL_PollEvent(&event))
    {
      // Map window coordinates to the logical 800x600 space
      SDL_ConvertEventToRenderCoordinates(renderer, &event);

      switch (event.type)
      {
      case SDL_EVENT_QUIT:
        running = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        isKeyDown[event.key.scancode] = true;

        if (!event.key.repeat)
        {
          isKeyPressed[event.key.scancode] = true;

          // F toggles fullscreen; it should not also act as "any key"
          if (event.key.key != SDLK_F)
          {
            anyKeyPressed = true;
          }

          // Inside the repeat guard, and that is the whole of this. Outside it,
          // holding F down did not toggle the window once: it toggled on every
          // auto-repeat the keyboard sent, about thirty a second, strobing
          // between fullscreen and windowed - each one an animated transition on
          // macOS - and writing the save file to disk every time round.
          if (event.key.key == SDLK_F)
          {
#if !defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE
            saveData.fullscreen = !saveData.fullscreen;
            SDL_SetWindowFullscreen(window, saveData.fullscreen);
            writeSave();
#endif
          }
        }
        break;
      case SDL_EVENT_KEY_UP:
        isKeyDown[event.key.scancode] = false;
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button < 8)
        {
          isMouseButtonDown[event.button.button] = true;
          isMousePressed[event.button.button] = true;
        }
        break;
      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button < 8)
        {
          isMouseButtonDown[event.button.button] = false;
        }
        break;
      case SDL_EVENT_MOUSE_MOTION:
        motionX = (int)event.motion.x;
        motionY = (int)event.motion.y;
        mouseMoved = true;
        break;
      }
    }

    // Inject scripted key presses (development helper)
    for (int i = 0; i < numKeyEvents; i++)
    {
      if (keyFrames[i] == frameCounter)
      {
        isKeyPressed[keyCodes[i]] = true;
        anyKeyPressed = true;
      }
    }

    // Update
    updateGameState();
    updateTransition();

    // Draw
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);

    SDL_RenderClear(renderer);

    drawGameState();
    drawVignette();
    drawTransition();

    if (scripted && frameCounter >= shotFrame && shotsTaken < shotFrames &&
        (frameCounter - shotFrame) % shotStep == 0)
    {
      char path[576];

      shotFilename(path, sizeof(path), shotPath, shotsTaken, shotFrames);
      saveShot(path, frameCounter);

      shotsTaken++;

      if (shotsTaken >= shotFrames)
      {
        quitRequested = true;
      }
    }

    frameCounter++;

    SDL_RenderPresent(renderer);

    // Cap the frame rate (fallback when VSync is unavailable). A scripted run
    // wants no part of this: it is stepping the world by a fixed amount, so
    // waiting for the clock to catch up buys it nothing but wall time.
    if (!scripted)
    {
      Uint64 elapsed = SDL_GetTicks() - startTime;
      float targetTime = 1000.f / FPS;
      if (targetTime > (float)elapsed)
      {
        SDL_Delay((Uint32)(targetTime - (float)elapsed));
      }
    }

    // Update previous motion
    prevMotionX = motionX;
    prevMotionY = motionY;

    // Update previous camera position
    camera.prevY = camera.y;

    clearFrameInput();

    paddleNextLevelUpdate();
    changeGameStateUpdate();
  }

  // A run that ends by closing the window is still a run. recordScore() lived
  // on the game over and win screens only, so the one exit from the game that
  // does not pass through either of them - the close button, mid-level, which
  // is how a session usually ends - threw the score away.
  if (getGameState() == GAME_STATE_PLAYING_SCREEN)
  {
    recordScore(paddle.score, paddle.level + 1);
  }

  // Cleanup
  writeSave();

  destroyGameState();
  destroyTransition();
  destroyLevelManager();
  destroyAudio();
  destroyFloatingTexts();
  destroyGfx();

  TTF_CloseFont(font16);
  TTF_CloseFont(font24);
  TTF_CloseFont(font32);
  TTF_CloseFont(font48);
  TTF_CloseFont(font64);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  if (mixer != NULL)
  {
    MIX_DestroyMixer(mixer);
  }
  MIX_Quit();

  TTF_Quit();
  SDL_Quit();

  return 0;
}
