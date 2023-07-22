#pragma once

#include "../globals.h"

void startTransition(void (*callback)(void));

void initializeTransition(void);

void updateTransition(void);

void drawTransition(void);

void destroyTransition(void);
