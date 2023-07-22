#include "brick.h"
#include "bricks.h"
#include "../level-types.h"
#include "../lib/camera.h"

static const int SHADOW_ALPHA = 40;
static const int SHADOW_OFFSET = 6;

const int BRICK_HEIGHT = LEVEL_PATTERN_ROW_HEIGHT;

static BrickProperty brickProps[4];

void initializeBrickProperties(void)
{
  brickProps[_] = (BrickProperty){(SDL_Color){0x00, 0x00, 0x00, 0x00}, 0};
  brickProps[B] = (BrickProperty){(SDL_Color){0xff, 0x00, 0x00, 0xff}, 100};
  brickProps[S] = (BrickProperty){(SDL_Color){0xff, 0xff, 0xff, 0xff}, 0};
  brickProps[F] = (BrickProperty){(SDL_Color){0x00, 0xff, 0x00, 0xff}, 500};
}

static void setColorAndValue(Brick *brick, const BrickType type)
{
  brick->color = brickProps[type].color;
  brick->value = brickProps[type].value;
}

static PaddleType getRandomPaddleType(void)
{
  const int randomValue = rand() % 20;

  if (randomValue <= 4)
  {
    PaddleType paddleTypes[] = {
        PADDLE_TYPE_LONG,
        PADDLE_TYPE_SHOOTING,
        PADDLE_TYPE_STICKY,
        PADDLE_TYPE_ADD_LIFE,
        PADDLE_TYPE_MORE_BALLS};

    return paddleTypes[randomValue];
  }

  return PADDLE_TYPE_NONE;
}

void initializeBrick(
    Brick *brick,
    const float x,
    const float y,
    const BrickType type)
{
  brick->pos.x = x;
  brick->pos.y = y;
  brick->type = type;
  brick->active = true;

  setColorAndValue(brick, type);

  PaddleType paddleType = getRandomPaddleType();

  if (paddleType != PADDLE_TYPE_NONE)
  {
    brick->brickItem = newBrickItem(
        x + BRICK_WIDTH / 2 - BRICK_ITEM_WIDTH / 2,
        y,
        paddleType);
  }
  else
  {
    brick->brickItem = NULL;
  }
}

void updateBrick(Brick *brick)
{
  updateBrickItem(brick->brickItem);

  if (!brick->active)
  {
    return;
  }
}

static void drawBrickShadow(const Brick *brick)
{
  Uint8 originalAlpha;

  SDL_GetTextureAlphaMod(bricksTexture, &originalAlpha);
  SDL_SetTextureAlphaMod(bricksTexture, SHADOW_ALPHA);

  SDL_FRect srcRect = {0, 0, BRICK_WIDTH, BRICK_HEIGHT};

  SDL_FRect dstRect = {
      brick->pos.x + SHADOW_OFFSET,
      brick->pos.y - camera.y + SHADOW_OFFSET,
      BRICK_WIDTH,
      BRICK_HEIGHT};

  SDL_RenderTexture(renderer, bricksTexture, &srcRect, &dstRect);

  SDL_SetTextureAlphaMod(bricksTexture, originalAlpha);
}

void drawBrick(const Brick *brick)
{
  drawBrickItem(brick->brickItem);

  if (!brick->active)
  {
    return;
  }

  drawBrickShadow(brick);

  SDL_FRect srcRect = {brick->type * BRICK_WIDTH, 0, BRICK_WIDTH, BRICK_HEIGHT};
  SDL_FRect dstRect = {
      brick->pos.x,
      brick->pos.y - camera.y,
      BRICK_WIDTH,
      BRICK_HEIGHT};
  SDL_RenderTexture(renderer, bricksTexture, &srcRect, &dstRect);
}

void destroyBrick(Brick *brick)
{
  if (brick->brickItem != NULL)
  {
    destroyBrickItem(brick->brickItem);
  }
}
