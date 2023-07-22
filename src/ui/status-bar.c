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
  SDL_RenderCopy(
      renderer,
      scoreText,
      NULL,
      &(SDL_Rect){20, 20, getSize(scoreText).x, getSize(scoreText).y});

  SDL_RenderCopy(
      renderer,
      livesText,
      NULL,
      &(SDL_Rect){
          SCREEN_WIDTH - getSize(livesText).x - 20,
          20,
          getSize(livesText).x,
          getSize(livesText).y});
}

void destroyStatusBar(void)
{
  SDL_DestroyTexture(scoreText);
  SDL_DestroyTexture(livesText);

  scoreText = NULL;
  livesText = NULL;
}
