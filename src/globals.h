#pragma once

// Platform detection
#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#define UNUSED
#else
#define UNUSED __attribute__((__unused__))
#endif

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#define assetsPath(filename) "" filename
#else
#define assetsPath(filename) "assets/" filename
#endif

#define SCREEN_WIDTH 800
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#define SCREEN_HEIGHT 1350
#else
#define SCREEN_HEIGHT 600
#endif
#define FPS 60

// Text textures are rasterized at TEXT_SCALE x and drawn at logical size,
// so they stay sharp on high-DPI displays. getSize() returns the logical size.
#define TEXT_SCALE 2

typedef struct Vec2
{
  float x, y;
} Vec2;

typedef struct Obj
{
  Vec2 pos;
  Vec2 vel;
  int speed;
  bool active;
} Obj;

typedef struct Timer
{
  float elapsedTime;
  float duration;
  void (*timeoutCallback)(void *userdata);
  void *userdata;
} Timer;

void updateTimer(Timer *timer);

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern MIX_Mixer *mixer;

extern TTF_Font *font16;
extern TTF_Font *font24;
extern TTF_Font *font32;
extern TTF_Font *font48;
extern TTF_Font *font64;

extern double dt;
extern float gameTime; // seconds since app start, for animations

#define K_LEFT SDL_SCANCODE_LEFT
#define K_RIGHT SDL_SCANCODE_RIGHT
#define K_SPACE SDL_SCANCODE_SPACE
#define K_UP SDL_SCANCODE_UP
#define K_DOWN SDL_SCANCODE_DOWN
#define K_RETURN SDL_SCANCODE_RETURN
#define K_ESCAPE SDL_SCANCODE_ESCAPE

extern bool isKeyDown[SDL_SCANCODE_COUNT];
extern bool isMouseButtonDown[8];

// Edge-triggered input: true only on the frame the key/button went down
extern bool isKeyPressed[SDL_SCANCODE_COUNT];
extern bool isMousePressed[8];
extern bool anyKeyPressed;
extern bool mouseMoved;

// Reset the per-frame (edge-triggered) input state; called once per frame
void clearFrameInput(void);

extern int motionX, motionY, prevMotionX, prevMotionY;

extern bool isPause;
extern bool quitRequested;

static inline int clampInt(int x, int a, int b)
{
  return x < a ? a : (x > b ? b : x);
}

static inline float clampFloat(float x, float a, float b)
{
  return x < a ? a : (x > b ? b : x);
}

static inline double clampDouble(double x, double a, double b)
{
  return x < a ? a : (x > b ? b : x);
}

// Still a macro, because the arguments arrive as ints in one caller and floats
// in the next, and the game reads better for `clamp` meaning one thing. But it
// hands them to a function now, so each is evaluated exactly once.
//
// The plain expansion named `x` three times and `b` twice. Nothing in the tree
// passes it an argument with a side effect in it today - `clamp(SDL_atoi(env),
// 1, getNumberOfLevels())` merely did the work three times over - and this is
// the kind of trap that is cheaper to close than to remember.
#define clamp(x, a, b)             \
  _Generic((x) + (a) + (b),        \
      float: clampFloat,           \
      double: clampDouble,         \
      long double: clampDouble,    \
      default: clampInt)((x), (a), (b))

bool checkCollision(
    float aX, float aY, int aW, int aH,
    float bX, float bY, int bW, int bH);

float lerp(float a, float b, float t);

// Ease-out with a small overshoot bounce (t in [0, 1])
float easeOutBack(float t);

// Standard cubic ease-out (t in [0, 1])
float easeOutCubic(float t);

// Random float in [0, 1)
float frand(void);

// Random float in [a, b)
float frandRange(float a, float b);

TTF_Font *loadFont(const char *file, float ptsize);

SDL_FPoint getSize(SDL_Texture *texture);

SDL_Texture *renderTextSolid(TTF_Font *font, const char *text, SDL_Color fg);

// Anti-aliased text; preferred for anything large or on-screen for a while
SDL_Texture *renderTextBlended(TTF_Font *font, const char *text, SDL_Color fg);

Vec2 vec2Norm(Vec2 vec, float m);
