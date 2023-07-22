#include "status-bar.h"
#include "../paddle/paddle.h"

static SDL_Texture *scoreText;
static SDL_Texture *livesText;

void updateScoreInStatusBar(void)
{
  char scoreStr[80];

  sprintf(scoreStr, "SCORE %d", paddle.score);

  scoreText = renderTextSolid(font24, scoreStr, (SDL_Color){255, 255, 255, 255});
}

void updateLivesInStatusBar(void)
{
  char livesStr[80];

  sprintf(livesStr, "LIVES %d", paddle.lives);

  livesText = renderTextSolid(font24, livesStr, (SDL_Color){255, 255, 255, 255});
}

void initializeStatusBar(void)
{
  updateScoreInStatusBar();
  updateLivesInStatusBar();
}

void drawStatusBar(void)
{
  SDL_FPoint scoreSize = getSize(scoreText);
  SDL_FPoint livesSize = getSize(livesText);

  SDL_FRect scoreRect = {20, 20, scoreSize.x, scoreSize.y};
  SDL_RenderTexture(renderer, scoreText, NULL, &scoreRect);

  SDL_FRect livesRect = {
      SCREEN_WIDTH - livesSize.x - 20,
      20,
      livesSize.x,
      livesSize.y};
  SDL_RenderTexture(renderer, livesText, NULL, &livesRect);
}

void destroyStatusBar(void)
{
  SDL_DestroyTexture(scoreText);
  SDL_DestroyTexture(livesText);

  scoreText = NULL;
  livesText = NULL;
}
