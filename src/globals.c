#include "globals.h"
#include "types.h"
#include "embedded_assets.h"
#include <string.h>

SDL_Window *window;
SDL_Renderer *renderer;
MIX_Mixer *mixer;

TTF_Font *font16;
TTF_Font *font24;
TTF_Font *font32;
TTF_Font *font48;
TTF_Font *font64;

double dt;
float gameTime;

bool isKeyDown[SDL_SCANCODE_COUNT];
bool isMouseButtonDown[8];

bool isKeyPressed[SDL_SCANCODE_COUNT];
bool isMousePressed[8];
bool anyKeyPressed;
bool mouseMoved;

int motionX, motionY, prevMotionX, prevMotionY;

bool isPause;
bool quitRequested;

const WorldTheme worldThemes[WORLD_COUNT + 1] = {
    // deep, accent, glow, name
    {{10, 16, 44, 255}, {30, 90, 150, 255}, {90, 200, 255, 255}, "CYAN DAWN"},
    {{8, 30, 24, 255}, {24, 110, 80, 255}, {110, 255, 170, 255}, "EMERALD DRIFT"},
    {{36, 14, 10, 255}, {130, 60, 24, 255}, {255, 170, 90, 255}, "EMBER FIELDS"},
    {{22, 10, 40, 255}, {90, 40, 140, 255}, {210, 130, 255, 255}, "VIOLET VOID"},
    {{12, 12, 34, 255}, {60, 50, 130, 255}, {140, 160, 255, 255}, "MENU"},
};

int worldForLevel(int level)
{
  int world = level / LEVELS_PER_WORLD;
  return clamp(world, 0, WORLD_COUNT - 1);
}

void clearFrameInput(void)
{
  memset(isKeyPressed, 0, sizeof(isKeyPressed));
  memset(isMousePressed, 0, sizeof(isMousePressed));
  anyKeyPressed = false;
  mouseMoved = false;
}

void updateTimer(Timer *timer)
{
  timer->elapsedTime += dt;

  if (timer->elapsedTime >= timer->duration)
  {
    if (timer->timeoutCallback)
    {
      timer->timeoutCallback(timer->userdata);
    }

    timer->elapsedTime = fmod(timer->elapsedTime, timer->duration);
  }
}

bool checkCollision(
    float aX, float aY, int aW, int aH,
    float bX, float bY, int bW, int bH)
{
  return (aX + aW > bX &&
          aX < bX + bW &&
          aY + aH > bY &&
          aY < bY + bH);
}

float lerp(float a, float b, float t)
{
  return (b - a) * t + a;
}

float easeOutBack(float t)
{
  const float c1 = 1.70158f;
  const float c3 = c1 + 1.0f;
  float u = t - 1.0f;
  return 1.0f + c3 * u * u * u + c1 * u * u;
}

float easeOutCubic(float t)
{
  float u = 1.0f - t;
  return 1.0f - u * u * u;
}

float frand(void)
{
  return (float)rand() / ((float)RAND_MAX + 1.0f);
}

float frandRange(float a, float b)
{
  return a + frand() * (b - a);
}

SDL_FPoint getSize(SDL_Texture *texture)
{
  SDL_FPoint size;
  float w, h;

  SDL_GetTextureSize(texture, &w, &h);

  // Text textures carry a supersample factor; report their logical size
  float scale = (float)SDL_GetNumberProperty(
      SDL_GetTextureProperties(texture), "breakup.scale", 1);

  size.x = w / scale;
  size.y = h / scale;

  return size;
}

TTF_Font *loadFont(const char *file, float ptsize)
{
  TTF_Font *font = NULL;
  const EmbeddedAsset *asset = findEmbeddedAsset(file);

  // Rasterize at TEXT_SCALE x; text textures are drawn at logical size
  ptsize *= TEXT_SCALE;

  if (asset != NULL)
  {
    SDL_IOStream *io = SDL_IOFromConstMem(asset->data, asset->size);
    font = TTF_OpenFontIO(io, true, ptsize);
  }
  else
  {
    font = TTF_OpenFont(file, ptsize);
  }

  if (font == NULL)
  {
    fprintf(stderr, "Unable to load font %s! SDL_ttf Error: %s\n", file, SDL_GetError());
    exit(1);
  }

  return font;
}

static SDL_Texture *textureFromTextSurface(SDL_Surface *surface)
{
  if (surface == NULL)
  {
    fprintf(stderr, "Unable to render text surface! SDL_ttf Error: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  if (texture == NULL)
  {
    fprintf(stderr, "Unable to create texture from rendered text! SDL_ttf Error: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_SetNumberProperty(SDL_GetTextureProperties(texture),
                        "breakup.scale", TEXT_SCALE);

  SDL_DestroySurface(surface);

  return texture;
}

SDL_Texture *renderTextSolid(TTF_Font *font, const char *text, SDL_Color fg)
{
  return textureFromTextSurface(TTF_RenderText_Solid(font, text, 0, fg));
}

SDL_Texture *renderTextBlended(TTF_Font *font, const char *text, SDL_Color fg)
{
  return textureFromTextSurface(TTF_RenderText_Blended(font, text, 0, fg));
}

Vec2 vec2Norm(Vec2 vec, float m)
{
  float length = sqrtf(vec.x * vec.x + vec.y * vec.y);

  if (length != 0)
  {
    vec.x = (vec.x / length) * m;
    vec.y = (vec.y / length) * m;
  }

  return vec;
}
