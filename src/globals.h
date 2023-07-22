#pragma once

#include <TargetConditionals.h>
#include <SDL2/SDL.h>
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#else
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#endif

#define UNUSED __attribute__((__unused__))

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#define assetsPath(filename) "" filename
#else
#define assetsPath(filename) "src/assets/" filename
#endif

static const int SCREEN_WIDTH = 800;
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
static const int SCREEN_HEIGHT = 1350;
#else
static const int SCREEN_HEIGHT = 600;
#endif
static const int FPS = 60;

typedef enum bool
{
  false,
  true
} bool;

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
  void (*timeoutCallback)(Obj *obj);
  Obj *obj;
} Timer;

void updateTimer(Timer *timer);

extern SDL_Renderer *renderer;

extern TTF_Font *font24;
extern TTF_Font *font32;

extern double dt;

static const int K_LEFT = 80;
static const int K_RIGHT = 79;
static const int K_SPACE = 44;

extern bool isKeyDown[255];
extern bool isMouseButtonDown[255];

extern int motionX, motionY, prevMotionX, prevMotionY;

extern bool isPause;

#define clamp(x, a, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

bool checkCollision(
    float aX, float aY, int aW, int aH,
    float bX, float bY, int bW, int bH);

float lerp(float a, float b, float t);

SDL_Surface *loadBMP(const char *file);

SDL_Texture *loadTexture(const char *file);

SDL_Point getSize(SDL_Texture *texture);

Mix_Chunk *loadWAV(const char *file);

SDL_Texture *renderTextSolid(TTF_Font *font, const char *text, SDL_Color fg);

void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

Vec2 vec2Norm(Vec2 vec, float m);
