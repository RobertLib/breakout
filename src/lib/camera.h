#pragma once

#include "../globals.h"

typedef struct Camera
{
  float y;
  float prevY;
  float shake; // current shake magnitude in pixels
} Camera;

extern Camera camera;

bool isCameraMoving(void);

// Adds impact shake (magnitudes accumulate up to a cap)
void addShake(float amount);

// Current vertical shake offset to apply while drawing the world
float shakeOffsetY(void);

void initializeCamera(void);

void updateCamera(void);
