#include "level-select-screen.h"
#include "../level-types.h"
#include "../level-manager.h"
#include "../lib/audio.h"
#include "../lib/game-state.h"
#include "../lib/gfx.h"
#include "../lib/save.h"
#include "../lib/starfield.h"
#include "../lib/transition.h"
#include "../paddle/paddle.h"
#include "../types.h"

#define GRID_COLS 5

#define CARD_W 132.0f
#define CARD_H 84.0f
#define CARD_GAP 12.0f
#define GRID_TOP 130.0f

static SDL_Texture *titleText;
static SDL_Texture *hintText;
static SDL_Texture **numberTexts;
static SDL_Texture **nameTexts;

static int selection;
static int levelCount;

static void gotoPlaying(void)
{
  setStartLevel(selection);
  nextGameState = GAME_STATE_PLAYING_SCREEN;
}

static void gotoMenu(void)
{
  nextGameState = GAME_STATE_MENU_SCREEN;
}

void initializeLevelSelectScreen(void)
{
  levelCount = getNumberOfLevels();
  selection = levelCount > 0 ? clamp(saveData.unlocked - 1, 0, levelCount - 1) : 0;

  titleText = renderTextBlended(font32, "SELECT LEVEL", (SDL_Color){255, 255, 255, 255});
  hintText = renderTextBlended(
      font16,
      "ENTER - PLAY    ESC - BACK",
      (SDL_Color){110, 120, 160, 255});

  // calloc(0, n) is allowed to hand back NULL, and a failed allocation looks
  // the same from here. Either way there is no grid to draw, and levelCount is
  // what every loop and every `% levelCount` below is written against - so it
  // is the one thing that has to agree with the arrays.
  if (levelCount > 0)
  {
    numberTexts = (SDL_Texture **)calloc((size_t)levelCount, sizeof(SDL_Texture *));
    nameTexts = (SDL_Texture **)calloc((size_t)levelCount, sizeof(SDL_Texture *));

    if (numberTexts == NULL || nameTexts == NULL)
    {
      fprintf(stderr, "Could not allocate the level select grid; showing none\n");
      free(numberTexts);
      free(nameTexts);
      numberTexts = NULL;
      nameTexts = NULL;
      levelCount = 0;
    }
  }

  for (int i = 0; i < levelCount; i++)
  {
    char num[8];
    snprintf(num, sizeof(num), "%d", i + 1);
    numberTexts[i] = renderTextBlended(font32, num, (SDL_Color){255, 255, 255, 255});
    nameTexts[i] = renderTextBlended(font16, getLevel(i)->name, (SDL_Color){190, 200, 230, 255});
  }

  playMusic(MUSIC_MENU);
}

static SDL_FRect cardRect(int index)
{
  int col = index % GRID_COLS;
  int row = index / GRID_COLS;

  float gridWidth = GRID_COLS * CARD_W + (GRID_COLS - 1) * CARD_GAP;
  float x0 = (SCREEN_WIDTH - gridWidth) / 2.0f;

  return (SDL_FRect){
      x0 + col * (CARD_W + CARD_GAP),
      GRID_TOP + row * (CARD_H + CARD_GAP),
      CARD_W,
      CARD_H};
}

void updateLevelSelectScreen(void)
{
  if (isTransitionActive())
  {
    return;
  }

  // Every arrow below is a `% levelCount`, which divides by zero on a build
  // whose levels did not load. There is nothing to select in that case anyway -
  // but ESC still has to work, because a screen you cannot leave is worse than
  // an empty one.
  if (levelCount <= 0)
  {
    if (isKeyPressed[K_ESCAPE] || isKeyPressed[K_RETURN] ||
        isKeyPressed[K_SPACE] || isMousePressed[1])
    {
      playSfx(SFX_MENU_SELECT);
      startTransition(gotoMenu);
    }

    return;
  }

  int prev = selection;

  if (isKeyPressed[K_RIGHT])
  {
    selection = (selection + 1) % levelCount;
  }
  if (isKeyPressed[K_LEFT])
  {
    selection = (selection + levelCount - 1) % levelCount;
  }
  if (isKeyPressed[K_DOWN])
  {
    selection = (selection + GRID_COLS) % levelCount;
  }
  if (isKeyPressed[K_UP])
  {
    selection = (selection + levelCount - GRID_COLS) % levelCount;
  }

  // Mouse hover / click
  for (int i = 0; i < levelCount; i++)
  {
    SDL_FRect rect = cardRect(i);

    if (motionX >= rect.x && motionX <= rect.x + rect.w &&
        motionY >= rect.y && motionY <= rect.y + rect.h)
    {
      if (mouseMoved)
      {
        selection = i;
      }

      if (isMousePressed[1] && i < saveData.unlocked)
      {
        selection = i;
        playSfx(SFX_MENU_SELECT);
        startTransition(gotoPlaying);
        return;
      }
    }
  }

  if (selection != prev)
  {
    playSfx(SFX_MENU_MOVE);
  }

  if (isKeyPressed[K_RETURN] || isKeyPressed[K_SPACE])
  {
    if (selection < saveData.unlocked)
    {
      playSfx(SFX_MENU_SELECT);
      startTransition(gotoPlaying);
    }
    else
    {
      playSfx(SFX_POWERUP_BAD);
    }
  }

  if (isKeyPressed[K_ESCAPE])
  {
    playSfx(SFX_MENU_SELECT);
    startTransition(gotoMenu);
  }
}

static void drawPadlock(float cx, float cy)
{
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 150, 155, 180, 220);

  // Body
  SDL_FRect body = {cx - 7, cy - 2, 14, 11};
  SDL_RenderFillRect(renderer, &body);

  // Shackle
  SDL_FRect left = {cx - 5, cy - 9, 2, 7};
  SDL_FRect right = {cx + 3, cy - 9, 2, 7};
  SDL_FRect top = {cx - 5, cy - 10, 10, 2};
  SDL_RenderFillRect(renderer, &left);
  SDL_RenderFillRect(renderer, &right);
  SDL_RenderFillRect(renderer, &top);

  // Keyhole
  SDL_SetRenderDrawColor(renderer, 40, 44, 66, 255);
  SDL_FRect hole = {cx - 1, cy + 1, 2, 5};
  SDL_RenderFillRect(renderer, &hole);
}

void drawLevelSelectScreen(void)
{
  drawBackground(WORLD_COUNT, gameTime * 24.0f);

  SDL_FPoint titleSize = getSize(titleText);
  SDL_FRect titleDst = {SCREEN_WIDTH / 2 - titleSize.x / 2, 55, titleSize.x, titleSize.y};
  SDL_RenderTexture(renderer, titleText, NULL, &titleDst);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  for (int i = 0; i < levelCount; i++)
  {
    SDL_FRect rect = cardRect(i);
    bool unlocked = i < saveData.unlocked;
    bool selected = i == selection;
    SDL_Color accent = worldThemes[worldForLevel(i)].glow;

    // Card background
    if (unlocked)
    {
      SDL_SetRenderDrawColor(renderer,
                             (Uint8)(accent.r * 0.16f),
                             (Uint8)(accent.g * 0.16f),
                             (Uint8)(accent.b * 0.16f),
                             225);
    }
    else
    {
      SDL_SetRenderDrawColor(renderer, 16, 18, 30, 225);
    }
    SDL_RenderFillRect(renderer, &rect);

    // Border
    if (selected)
    {
      float pulse = 0.6f + 0.4f * sinf(gameTime * 6.0f);
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, (Uint8)(160 + 90 * pulse));

      SDL_FRect outer = {rect.x - 2, rect.y - 2, rect.w + 4, rect.h + 4};
      SDL_RenderRect(renderer, &outer);
      SDL_RenderRect(renderer, &rect);

      // Glow behind the selected card
      SDL_SetTextureColorMod(texGlow, accent.r, accent.g, accent.b);
      SDL_SetTextureAlphaMod(texGlow, 70);
      SDL_FRect glow = {rect.x - 30, rect.y - 30, rect.w + 60, rect.h + 60};
      SDL_RenderTexture(renderer, texGlow, NULL, &glow);
      SDL_SetTextureColorMod(texGlow, 255, 255, 255);
      SDL_SetTextureAlphaMod(texGlow, 255);
    }
    else
    {
      SDL_SetRenderDrawColor(renderer, accent.r, accent.g, accent.b,
                             unlocked ? 140 : 50);
      SDL_RenderRect(renderer, &rect);
    }

    if (unlocked)
    {
      SDL_FPoint numSize = getSize(numberTexts[i]);
      SDL_FRect numDst = {
          rect.x + rect.w / 2 - numSize.x / 2,
          rect.y + 12,
          numSize.x,
          numSize.y};
      SDL_RenderTexture(renderer, numberTexts[i], NULL, &numDst);

      // Scale the name down if it is wider than the card
      SDL_FPoint nameSize = getSize(nameTexts[i]);
      float maxW = rect.w - 10;
      float scale = nameSize.x > maxW ? maxW / nameSize.x : 1.0f;
      float w = nameSize.x * scale;
      float h = nameSize.y * scale;

      SDL_FRect nameDst = {
          rect.x + rect.w / 2 - w / 2,
          rect.y + rect.h - 16 - h / 2,
          w,
          h};
      SDL_RenderTexture(renderer, nameTexts[i], NULL, &nameDst);
    }
    else
    {
      SDL_SetTextureAlphaMod(numberTexts[i], 70);
      SDL_FPoint numSize = getSize(numberTexts[i]);
      SDL_FRect numDst = {
          rect.x + rect.w / 2 - numSize.x / 2,
          rect.y + 8,
          numSize.x,
          numSize.y};
      SDL_RenderTexture(renderer, numberTexts[i], NULL, &numDst);
      SDL_SetTextureAlphaMod(numberTexts[i], 255);

      drawPadlock(rect.x + rect.w / 2, rect.y + rect.h - 24);
    }
  }

  SDL_FPoint hintSize = getSize(hintText);
  SDL_FRect hintDst = {SCREEN_WIDTH / 2 - hintSize.x / 2, SCREEN_HEIGHT - 30, hintSize.x, hintSize.y};
  SDL_RenderTexture(renderer, hintText, NULL, &hintDst);
}

void destroyLevelSelectScreen(void)
{
  SDL_DestroyTexture(titleText);
  SDL_DestroyTexture(hintText);
  titleText = NULL;
  hintText = NULL;

  // levelCount is zeroed above when either array is missing, so this loop and
  // the arrays it walks cannot disagree - but the guard is here because the
  // free() below is written to cope with NULL and this is not.
  if (numberTexts != NULL && nameTexts != NULL)
  {
    for (int i = 0; i < levelCount; i++)
    {
      SDL_DestroyTexture(numberTexts[i]);
      SDL_DestroyTexture(nameTexts[i]);
    }
  }

  free(numberTexts);
  free(nameTexts);
  numberTexts = NULL;
  nameTexts = NULL;
}
