#include "globals.h"

SDL_Renderer *renderer;

TTF_Font *font24;
TTF_Font *font32;

double dt;

bool isKeyDown[255];
bool isMouseButtonDown[255];

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
  SDL_Surface *surface = SDL_LoadBMP(file);

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
  SDL_Surface *loadedSurface = SDL_LoadBMP(file);

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
    SDL_FreeSurface(loadedSurface);
  }

  return newTexture;
}

SDL_Point getSize(SDL_Texture *texture)
{
  SDL_Point size;

  SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);

  return size;
}

Mix_Chunk *loadWAV(const char *file)
{
  Mix_Chunk *chunk = Mix_LoadWAV(file);

  if (chunk == NULL)
  {
    fprintf(stderr, "Unable to load wav %s! SDL_mixer Error: %s\n", file, Mix_GetError());
    exit(1);
  }

  return chunk;
}

SDL_Texture *renderTextSolid(TTF_Font *font, const char *text, SDL_Color fg)
{
  SDL_Surface *surface = TTF_RenderText_Solid(font, text, fg);

  if (surface == NULL)
  {
    fprintf(stderr, "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    exit(1);
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  if (texture == NULL)
  {
    fprintf(stderr, "Unable to create texture from rendered text! SDL_ttf Error: %s\n", TTF_GetError());
    exit(1);
  }

  SDL_FreeSurface(surface);

  return texture;
}

void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  if (x >= 0 && x < surface->w && y >= 0 && y < surface->h)
  {
    Uint32 *targetPixel = (Uint32 *)((Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
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
