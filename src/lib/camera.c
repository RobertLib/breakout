#include "camera.h"
#include "../paddle/paddle.h"

Camera camera;

bool isCameraMoving(void)
{
  return camera.y != camera.prevY;
}

void addShake(float amount)
{
  camera.shake = fminf(camera.shake + amount, 18.0f);
}

float shakeOffsetY(void)
{
  if (camera.shake <= 0.1f)
  {
    return 0;
  }

  return frandRange(-camera.shake, camera.shake);
}

void initializeCamera(void)
{
  // Position the camera so the paddle sits near the bottom of the screen
  float startY = paddle.pos.y + PADDLE_HEIGHT + 30.0f - SCREEN_HEIGHT;
  camera.y = startY < 0.0f ? 0.0f : startY;
  camera.prevY = camera.y;
  camera.shake = 0;
}

void updateCamera(void)
{
  if (camera.y <= 0)
  {
    camera.y = 0;
    camera.prevY = 0;
  }

  // Shake decays quickly
  camera.shake = fmaxf(0.0f, camera.shake - 40.0f * (float)dt);
}
