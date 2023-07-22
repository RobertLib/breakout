#pragma once

#include "../bullet/bullet.h"
#include "../globals.h"

#define PADDLE_HEIGHT 20
#define NUMBER_OF_BULLETS 10

typedef enum PaddleType
{
  PADDLE_TYPE_DEFAULT,
  PADDLE_TYPE_LONG,
  PADDLE_TYPE_SHOOTING,
  PADDLE_TYPE_STICKY,
  PADDLE_TYPE_ADD_LIFE,
  PADDLE_TYPE_MORE_BALLS,
  PADDLE_TYPE_DYING,
  PADDLE_TYPE_NONE,
} PaddleType;

typedef struct Paddle
{
  Vec2 pos;
  Vec2 vel;
  Vec2 moveTo;
  int score;
  int lives;
  int level;
  bool nextLevel;
  bool isReady;
  bool isTimeAfterStart;
  PaddleType type;
  Bullet bullets[NUMBER_OF_BULLETS];
  bool canShoot;
  bool inCollision;
  bool levelCompleted;
  SDL_Texture *texture;
} Paddle;

extern Paddle paddle;

void addScore(const int value);

void setStartLevel(int level);

void changePaddleType(PaddleType type);

void paddleNextLevelUpdate(void);

void paddleTimerUpdate(void);

int paddleWidth(void);

void initializePaddle(void);

void updatePaddle(void);

void drawPaddle(void);

void destroyPaddle(void);
