#include "brick-item.h"
#include "bricks.h"
#include "../lib/camera.h"

static const int SPEED = 200;
static const int NUMBER_OF_ANIMATION_FRAMES = 4;
static const int SPIN_ANIMATION_SPEED = 10;

BrickItem *newBrickItem(const float x, const float y, const PaddleType type)
{
  BrickItem *brickItem = (BrickItem *)malloc(sizeof(BrickItem));

  if (brickItem == NULL)
  {
    fprintf(stderr, "Memory not allocated.\n");
    exit(1);
  }

  brickItem->pos.x = x;
  brickItem->pos.y = y;
  brickItem->active = false;
  brickItem->type = type;

  return brickItem;
}

void updateBrickItem(BrickItem *brickItem)
{
  if (brickItem == NULL || !brickItem->active)
  {
    return;
  }

  brickItem->pos.y += SPEED * dt;

  // If the brick item is out of the screen, deactivate it
  if (brickItem->pos.y > camera.y + SCREEN_HEIGHT)
  {
    brickItem->active = false;
  }

  // Check if the brick item collides with the paddle
  if (checkCollision(
          brickItem->pos.x,
          brickItem->pos.y,
          BRICK_ITEM_WIDTH,
          BRICK_ITEM_HEIGHT,
          paddle.pos.x - paddleWidth() / 2,
          paddle.pos.y,
          paddleWidth(),
          PADDLE_HEIGHT))
  {
    brickItem->active = false;

    changePaddleType(brickItem->type);
  }

  // Update the animation timer
  brickItem->timer += SPIN_ANIMATION_SPEED * dt;

  if (brickItem->timer > NUMBER_OF_ANIMATION_FRAMES)
  {
    brickItem->timer = 0;
  }
}

void drawBrickItem(BrickItem *brickItem)
{
  if (brickItem == NULL || !brickItem->active)
  {
    return;
  }

  SDL_FRect srcRect = {
      (brickItem->type - 1) * BRICK_ITEM_WIDTH,
      (int)brickItem->timer * BRICK_ITEM_HEIGHT,
      BRICK_ITEM_WIDTH,
      BRICK_ITEM_HEIGHT};
  SDL_FRect dstRect = {
      brickItem->pos.x,
      brickItem->pos.y - camera.y,
      BRICK_ITEM_WIDTH,
      BRICK_ITEM_HEIGHT};
  SDL_RenderTexture(renderer, brickItemsTexture, &srcRect, &dstRect);
}

void destroyBrickItem(BrickItem *brickItem)
{
  free(brickItem);
}
