#include "globals.h"
#include "embedded_assets.h"
#include <string.h>

SDL_Renderer *renderer;
MIX_Mixer *mixer;

TTF_Font *font16;
TTF_Font *font24;
TTF_Font *font32;

double dt;

bool isKeyDown[SDL_SCANCODE_COUNT];
bool isMouseButtonDown[255];
bool ePressed = false;

int motionX, motionY, prevMotionX, prevMotionY;

bool isPause;

void updateTimer(Timer *timer)
{
  timer->elapsedTime += dt;

  if (timer->elapsedTime >= timer->duration)
  {
    if (timer->timeoutCallback)
    {
      timer->timeoutCallback(timer->obj);
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

SDL_Surface *loadBMP(const char *file)
{
  SDL_Surface *surface = NULL;
  const EmbeddedAsset *asset = findEmbeddedAsset(file);

  if (asset != NULL)
  {
    SDL_IOStream *io = SDL_IOFromConstMem(asset->data, asset->size);
    surface = SDL_LoadBMP_IO(io, true);
  }
  else
  {
    surface = SDL_LoadBMP(file);
  }

  if (surface == NULL)
  {
    fprintf(stderr, "Unable to load image %s! SDL Error: %s\n", file, SDL_GetError());
    exit(1);
  }

  return surface;
}

SDL_Texture *loadTexture(const char *file)
{
  // The final texture
  SDL_Texture *newTexture = NULL;

  // Load image at specified path
  SDL_Surface *loadedSurface = loadBMP(file);

  if (loadedSurface == NULL)
  {
    fprintf(stderr, "Unable to load image %s! SDL Error: %s\n", file, SDL_GetError());
  }
  else
  {
    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

    if (newTexture == NULL)
    {
      fprintf(stderr, "Unable to create texture from %s! SDL Error: %s\n", file, SDL_GetError());
    }

    // Get rid of old loaded surface
    SDL_DestroySurface(loadedSurface);
  }

  return newTexture;
}

SDL_FPoint getSize(SDL_Texture *texture)
{
  SDL_FPoint size;
  float w, h;

  SDL_GetTextureSize(texture, &w, &h);
  size.x = w;
  size.y = h;

  return size;
}

TTF_Font *loadFont(const char *file, float ptsize)
{
  TTF_Font *font = NULL;
  const EmbeddedAsset *asset = findEmbeddedAsset(file);

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

MIX_Audio *loadWAV(const char *file)
{
  MIX_Audio *audio = NULL;
  const EmbeddedAsset *asset = findEmbeddedAsset(file);

  if (asset != NULL)
  {
    SDL_IOStream *io = SDL_IOFromConstMem(asset->data, asset->size);
    audio = MIX_LoadAudio_IO(mixer, io, true, true);
  }
  else
  {
    audio = MIX_LoadAudio(mixer, file, true);
  }

  if (audio == NULL)
  {
    fprintf(stderr, "Unable to load wav %s! SDL_mixer Error: %s\n", file, SDL_GetError());
    exit(1);
  }

  return audio;
}

void playSound(MIX_Audio *audio)
{
  if (audio != NULL)
  {
    MIX_PlayAudio(mixer, audio);
  }
}

SDL_Texture *renderTextSolid(TTF_Font *font, const char *text, SDL_Color fg)
{
  SDL_Surface *surface = TTF_RenderText_Solid(font, text, 0, fg);

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

  SDL_DestroySurface(surface);

  return texture;
}

void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  if (x >= 0 && x < surface->w && y >= 0 && y < surface->h)
  {
    int bytesPerPixel = SDL_GetPixelFormatDetails(surface->format)->bytes_per_pixel;
    Uint32 *targetPixel = (Uint32 *)((Uint8 *)surface->pixels + y * surface->pitch + x * bytesPerPixel);
    *targetPixel = pixel;
  }
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
