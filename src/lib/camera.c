#include "camera.h"
#include "../level-types.h"

Camera camera;

bool isCameraMoving(void)
{
  return camera.y != camera.prevY;
}

void initializeCamera(void)
{
  camera.y = LEVEL_HEIGHT - SCREEN_HEIGHT;
  camera.prevY = camera.y;
}

void updateCamera(void)
{
  if (camera.y <= 0)
  {
    camera.y = 0;
    camera.prevY = 0;
  }
}
