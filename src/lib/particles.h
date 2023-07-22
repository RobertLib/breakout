#pragma once

#include "../globals.h"

// World-space particle system (drawn relative to the camera).

void clearParticles(void);

void updateParticles(void);

void drawParticles(void);

// Colored explosion: glowing sparks + tumbling shards
void spawnBurst(float x, float y, SDL_Color color, int count, float speed);

// Single soft glow that expands and fades (impacts, muzzle flashes)
void spawnGlowPuff(float x, float y, SDL_Color color, float size, float life);

// Small trail dot behind the ball
void spawnTrail(float x, float y, SDL_Color color, float size);

// Big celebratory firework (win screen)
void spawnFirework(float x, float y);
