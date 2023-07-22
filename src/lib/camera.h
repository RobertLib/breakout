#pragma once

#include "../globals.h"

typedef struct Camera
{
  float y, prevY;
} Camera;

extern Camera camera;

bool isCameraMoving(void);

void initializeCamera(void);

void updateCamera(void);
