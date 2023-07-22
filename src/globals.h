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
#define assetsPath(filename) "src/assets/" filename
#endif

#define SCREEN_WIDTH 800
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#define SCREEN_HEIGHT 1350
#else
#define SCREEN_HEIGHT 600
#endif
#define FPS 60

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
extern MIX_Mixer *mixer;

extern TTF_Font *font16;
extern TTF_Font *font24;
extern TTF_Font *font32;

extern double dt;

static const SDL_Scancode K_LEFT = SDL_SCANCODE_LEFT;
static const SDL_Scancode K_RIGHT = SDL_SCANCODE_RIGHT;
static const SDL_Scancode K_SPACE = SDL_SCANCODE_SPACE;

extern bool isKeyDown[SDL_SCANCODE_COUNT];
extern bool isMouseButtonDown[255];
extern bool ePressed; // Global state for E key to toggle editor

extern int motionX, motionY, prevMotionX, prevMotionY;

extern bool isPause;

#define clamp(x, a, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))

bool checkCollision(
    float aX, float aY, int aW, int aH,
    float bX, float bY, int bW, int bH);

float lerp(float a, float b, float t);

SDL_Surface *loadBMP(const char *file);

SDL_Texture *loadTexture(const char *file);

SDL_FPoint getSize(SDL_Texture *texture);

TTF_Font *loadFont(const char *file, float ptsize);

MIX_Audio *loadWAV(const char *file);

void playSound(MIX_Audio *audio);

SDL_Texture *renderTextSolid(TTF_Font *font, const char *text, SDL_Color fg);

void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

Vec2 vec2Norm(Vec2 vec, float m);
