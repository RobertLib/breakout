#include "ball.h"
#include "balls.h"
#include "bricks.h"
#include "camera.h"
#include "paddle.h"
#include "playing.h"
#include "status-bar.h"

static const int INIT_SPEED = 400;
static const int MAX_SPEED = 600;
static const int INCREASING_SPEED = 5;
static const float SPEED_INCREASE_FACTOR = 1.25;

static void increaseSpeed(Ball *ball)
{
  if (ball->speed < MAX_SPEED)
  {
    ball->speed += INCREASING_SPEED;
  }
}

static void collisionWithBrick(Ball *ball, Brick *brick)
{
  if (brick == NULL)
  {
    fprintf(stderr, "Error: brick is null.\n");
    return;
  }

  if (brick->value == 0)
  {
    return;
  }

  brick->active = false;

  if (brick->brickItem != NULL)
  {
    brick->brickItem->active = true;
  }

  addScore(brick->value);
  updateScoreInStatusBar();
  increaseSpeed(ball);

  if (areAllFinalBricksInactive())
  {
    paddle.levelCompleted = true;
  }
}

static void placeOnPaddle(Ball *ball)
{
  ball->pos.x = paddle.pos.x + ball->dockedAt.x;
  ball->pos.y = paddle.pos.y + ball->dockedAt.y;
  ball->vel.x = 0;
  ball->vel.y = 0;
  ball->speed = 0;
}

bool isBallDocked(const Ball *ball)
{
  return ball->dockedAt.x != 0 && ball->dockedAt.y != 0;
}

void releaseBall(Ball *ball)
{
  ball->vel.x = INIT_SPEED - rand() % INIT_SPEED * 2;
  ball->vel.y = -INIT_SPEED;
  ball->speed = INIT_SPEED;
  ball->dockedAt.x = 0;
  ball->dockedAt.y = 0;
}

void initializeBall(Ball *ball, bool active)
{
  ball->active = active;
  ball->dockedAt.x = -BALL_SIZE / 2;
  ball->dockedAt.y = -BALL_SIZE;

  placeOnPaddle(ball);
}

static void handleCollision(Ball *ball, Brick *brick, const char direction)
{
  if (direction == 'x')
  {
    ball->vel.x *= -1;
  }
  else if (direction == 'y')
  {
    ball->vel.y *= -1;
  }

  collisionWithBrick(ball, brick);

  // Play the sound only if the ball is not docked
  if (!isBallDocked(ball))
  {
    Mix_PlayChannel(1, ballSound, 0);
  }
}

static void handleSurroundingTiles(Obj *ball, Brick *brick)
{
  if (checkCollision(
          ball->pos.x + ball->vel.x * dt,
          ball->pos.y,
          BALL_SIZE,
          BALL_SIZE,
          brick->pos.x,
          brick->pos.y,
          BRICK_WIDTH,
          BRICK_HEIGHT))
  {
    handleCollision((Ball *)ball, brick, 'x');
  }

  if (checkCollision(
          ball->pos.x,
          ball->pos.y + ball->vel.y * dt,
          BALL_SIZE,
          BALL_SIZE,
          brick->pos.x,
          brick->pos.y,
          BRICK_WIDTH,
          BRICK_HEIGHT))
  {
    handleCollision((Ball *)ball, brick, 'y');
  }
}

static void dockBall(Ball *ball)
{
  ball->dockedAt.x = ball->pos.x - paddle.pos.x - BALL_SIZE / 2;
  ball->dockedAt.y = ball->pos.y - paddle.pos.y - BALL_SIZE;

  int halfWidth = paddleWidth() / 2;

  if (ball->dockedAt.x < -halfWidth - BALL_SIZE)
  {
    ball->dockedAt.x = -halfWidth - BALL_SIZE;
  }
  else if (ball->dockedAt.x > halfWidth)
  {
    ball->dockedAt.x = halfWidth;
  }

  if (ball->dockedAt.y < -BALL_SIZE)
  {
    ball->dockedAt.y = -BALL_SIZE;
  }
}

static void bounceBallOffPaddle(Ball *ball)
{
  float ballToPaddlePosX = (ball->pos.x + BALL_SIZE / 2) - paddle.pos.x;
  float impactFactor = ballToPaddlePosX / paddleWidth() * 2;

  ball->vel.x += impactFactor * ball->speed + paddle.vel.x / 4;
  ball->pos.x += paddle.vel.x * dt;

  if (paddle.type == PADDLE_TYPE_STICKY)
  {
    dockBall(ball);
  }
}

void updateBall(Ball *ball)
{
  if (!ball->active)
  {
    return;
  }

  float nextX = ball->pos.x + ball->vel.x * dt;
  float nextY = ball->pos.y + ball->vel.y * dt;

  // Check if the ball is out of the screen
  if (nextX < 0 || nextX + BALL_SIZE > SCREEN_WIDTH)
  {
    ball->vel.x *= -1;
  }

  if (nextY < camera.y)
  {
    ball->vel.y *= -1;
  }

  checkSurroundingTilesInGrid((Obj *)ball, handleSurroundingTiles);

  // Check if the ball collides with the paddle
  if (checkCollision(
          nextX,
          nextY,
          BALL_SIZE,
          BALL_SIZE,
          paddle.pos.x - paddleWidth() / 2 + paddle.vel.x * dt,
          paddle.pos.y + paddle.vel.y * dt,
          paddleWidth(),
          PADDLE_HEIGHT))
  {
    ball->vel.y *= -1;

    bounceBallOffPaddle(ball);

    // Play the sound only if the ball is not docked
    if (!isBallDocked(ball))
    {
      Mix_PlayChannel(1, ballSound, 0);
    }
  }

  if (
      ball->vel.y < 0 &&
      ball->pos.y < camera.y + SCREEN_HEIGHT / 4 &&
      camera.y > 0)
  {
    // Move the background, camera and the paddle
    backgroundPosY -= ball->vel.y / 4 * dt;
    camera.y += ball->vel.y * dt;
    paddle.pos.y += ball->vel.y * dt;
  }

  if (isBallDocked(ball))
  {
    placeOnPaddle(ball);

    // Release the ball
    if ((isKeyDown[K_SPACE] || isMouseButtonDown[1]) &&
        paddle.isReady &&
        !paddle.inCollision &&
        !isCameraMoving())
    {
      releaseBall(ball);
    }

    return;
  }

  if (fabs(ball->vel.y) < INIT_SPEED / 4)
  {
    // Increase the speed of the ball
    ball->vel.y *= SPEED_INCREASE_FACTOR;
  }

  // Normalize the velocity
  ball->vel = vec2Norm(ball->vel, ball->speed);

  ball->pos.x += ball->vel.x * dt;
  ball->pos.y += ball->vel.y * dt;
}

void drawBall(const Ball *ball)
{
  if (!ball->active)
  {
    return;
  }

  if (isBallDocked(ball) && isCameraMoving())
  {
    SDL_SetTextureAlphaMod(ballTexture, 255 / 2);
  }
  else
  {
    SDL_SetTextureAlphaMod(ballTexture, 255);
  }

  SDL_RenderCopy(
      renderer,
      ballTexture,
      NULL,
      &(SDL_Rect){ball->pos.x, ball->pos.y - camera.y, BALL_SIZE, BALL_SIZE});

  SDL_SetTextureAlphaMod(ballTexture, 255);
}
