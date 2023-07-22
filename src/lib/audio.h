#pragma once

#include "../globals.h"
#include "../types.h"

// All audio is synthesized at startup — no audio files needed.

typedef enum Sfx
{
  SFX_BALL_PADDLE,
  SFX_BALL_WALL,
  SFX_BRICK_HIT,   // brick damaged but not destroyed
  SFX_BRICK_BREAK,
  SFX_EXPLOSION,
  SFX_GOLD,
  SFX_CRYSTAL,
  SFX_POWERUP_GOOD,
  SFX_POWERUP_BAD,
  SFX_SHOOT,
  SFX_ENEMY_HIT,
  SFX_LIFE_LOST,
  SFX_LEVEL_COMPLETE,
  SFX_MENU_MOVE,
  SFX_MENU_SELECT,
  SFX_SHIELD,
  SFX_CATCH,
  SFX_GAME_OVER, // jingle
  SFX_WIN,       // jingle
  SFX_COUNT
} Sfx;

#define GAME_TRACKS_PER_WORLD 2

typedef enum MusicTrack
{
  MUSIC_NONE = -1,
  MUSIC_MENU,
  MUSIC_GAME_FIRST,
  // GAME_TRACKS_PER_WORLD tracks per world follow MUSIC_GAME_FIRST
  MUSIC_COUNT = MUSIC_GAME_FIRST + WORLD_COUNT * GAME_TRACKS_PER_WORLD
} MusicTrack;

// Each world has its own pair of tracks and levels alternate between them,
// so no two consecutive levels play the same music
MusicTrack musicForLevel(int level);

void initializeAudio(void);

void destroyAudio(void);

void playSfx(Sfx sfx);

// Starts a looping music track; no-op if it is already playing
void playMusic(MusicTrack track);

void stopMusic(void);

void setMusicVolume(float v); // 0..1
void setSfxVolume(float v);   // 0..1
