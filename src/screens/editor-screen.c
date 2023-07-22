#include "editor-screen.h"
#include "../globals.h"
#include "../level-manager.h"
#include "../level-types.h"
#include "../bricks/brick.h"
#include "../lib/game-state.h"
#include "../lib/camera.h"
#include "../paddle/paddle.h"
#include <string.h>

static Level editorLevel;
static int currentBrickType = B; // Default to basic brick
static int currentLevelIndex = 0;
static bool isDragging = false;
static SDL_Texture *helpText = NULL;
static SDL_Texture *helpText2 = NULL;
static SDL_Texture *levelInfoText = NULL;
static SDL_Texture *brickTypeText = NULL;
static float editorCameraY = 0.0f;
static const float CAMERA_SCROLL_SPEED = 500.0f;
static bool cameFromGame = false;
static bool sPressed = false;
static bool lPressed = false;
static bool nPressed = false;
static bool upPressed = false;
static bool downPressed = false;
static bool leftPressed = false;
static bool rightPressed = false;

// Calculate grid position from mouse coordinates
static void getGridPosition(int mouseX, int mouseY, int *gridX, int *gridY)
{
  *gridX = (mouseX - LEVEL_PATTERN_INDENT) / (BRICK_WIDTH + LEVEL_PATTERN_SPACING);
  *gridY = ((int)(mouseY + editorCameraY) - LEVEL_PATTERN_INDENT) / (LEVEL_PATTERN_ROW_HEIGHT + LEVEL_PATTERN_SPACING);
}

// Get array index from grid position
static int getPatternIndex(int gridX, int gridY)
{
  if (gridX < 0 || gridX >= LEVEL_PATTERN_COLS ||
      gridY < 0 || gridY >= LEVEL_PATTERN_ROWS)
  {
    return -1;
  }
  return gridY * LEVEL_PATTERN_COLS + gridX;
}

// Update text displays
static void updateTextDisplays(void)
{
  char buffer[256];

  // Update level info
  if (levelInfoText != NULL)
  {
    SDL_DestroyTexture(levelInfoText);
  }
  snprintf(buffer, sizeof(buffer), "Lvl:%d/%d", currentLevelIndex + 1, getNumberOfLevels());
  levelInfoText = renderTextSolid(font16, buffer, (SDL_Color){0, 0, 0, 255});

  // Update brick type info
  if (brickTypeText != NULL)
  {
    SDL_DestroyTexture(brickTypeText);
  }
  const char *typeNames[] = {"Empty", "Basic", "Stone", "Final", "Enemy"};
  snprintf(buffer, sizeof(buffer), "%s", typeNames[currentBrickType]);
  brickTypeText = renderTextSolid(font16, buffer, (SDL_Color){0, 0, 0, 255});
}

void initializeEditorScreen(void)
{
  // Reset key states
  sPressed = false;
  lPressed = false;
  nPressed = false;
  upPressed = false;
  downPressed = false;
  leftPressed = false;
  rightPressed = false;

  // If currentLevelIndex wasn't set by setEditorLevel, start at 0
  if (!cameFromGame)
  {
    currentLevelIndex = 0;
  }

  // Load the current level
  if (getNumberOfLevels() > 0 && currentLevelIndex < getNumberOfLevels())
  {
    memcpy(&editorLevel, &getLevels()[currentLevelIndex], sizeof(Level));
  }
  else
  {
    // Create blank level
    memset(&editorLevel, _, sizeof(Level));
  }

  currentBrickType = B;
  isDragging = false;

  // Create help text (two lines)
  helpText = renderTextSolid(font16,
    "1-5:Brick N:New L:Load S:Save E:Exit",
    (SDL_Color){0, 0, 0, 255});

  helpText2 = renderTextSolid(font16,
    "Arrows: Left/Right=Switch Level Up/Down=Scroll",
    (SDL_Color){0, 0, 0, 255});

  editorCameraY = 0.0f;
  updateTextDisplays();
}

void setEditorLevel(int levelIndex)
{
  if (levelIndex >= 0 && levelIndex < getNumberOfLevels())
  {
    currentLevelIndex = levelIndex;
    memcpy(&editorLevel, &getLevels()[currentLevelIndex], sizeof(Level));
    editorCameraY = 0.0f;
    cameFromGame = true;
    updateTextDisplays();
  }
}

void updateEditorScreen(void)
{
  // Handle keyboard input for brick type selection
  if (isKeyDown[SDL_SCANCODE_1])
  {
    currentBrickType = _;
    updateTextDisplays();
  }
  else if (isKeyDown[SDL_SCANCODE_2])
  {
    currentBrickType = B;
    updateTextDisplays();
  }
  else if (isKeyDown[SDL_SCANCODE_3])
  {
    currentBrickType = S;
    updateTextDisplays();
  }
  else if (isKeyDown[SDL_SCANCODE_4])
  {
    currentBrickType = F;
    updateTextDisplays();
  }
  else if (isKeyDown[SDL_SCANCODE_5])
  {
    currentBrickType = E;
    updateTextDisplays();
  }

  // Handle camera scrolling with Up/Down arrows
  if (isKeyDown[SDL_SCANCODE_UP])
  {
    editorCameraY -= CAMERA_SCROLL_SPEED * dt;
    if (editorCameraY < 0)
    {
      editorCameraY = 0;
    }
  }

  if (isKeyDown[SDL_SCANCODE_DOWN])
  {
    editorCameraY += CAMERA_SCROLL_SPEED * dt;
    float maxCameraY = LEVEL_HEIGHT - SCREEN_HEIGHT;
    if (editorCameraY > maxCameraY)
    {
      editorCameraY = maxCameraY;
    }
  }

  // Handle level navigation with Left/Right arrows

  if (isKeyDown[K_LEFT] && !leftPressed)
  {
    leftPressed = true;
    if (currentLevelIndex > 0)
    {
      // Save current level
      setLevel(currentLevelIndex, &editorLevel);

      currentLevelIndex--;
      memcpy(&editorLevel, &getLevels()[currentLevelIndex], sizeof(Level));
      editorCameraY = 0.0f;
      // Keep cameFromGame state when switching levels
      updateTextDisplays();
    }
  }
  else if (!isKeyDown[K_LEFT])
  {
    leftPressed = false;
  }

  if (isKeyDown[K_RIGHT] && !rightPressed)
  {
    rightPressed = true;
    if (currentLevelIndex < getNumberOfLevels() - 1)
    {
      // Save current level
      setLevel(currentLevelIndex, &editorLevel);

      currentLevelIndex++;
      memcpy(&editorLevel, &getLevels()[currentLevelIndex], sizeof(Level));
      editorCameraY = 0.0f;
      // Keep cameFromGame state when switching levels
      updateTextDisplays();
    }
  }
  else if (!isKeyDown[K_RIGHT])
  {
    rightPressed = false;
  }

  // Handle E to exit editor
  if (isKeyDown[SDL_SCANCODE_E] && !ePressed)
  {
    ePressed = true;

    // Save current level
    setLevel(currentLevelIndex, &editorLevel);

    // Set the start level for the game
    setStartLevel(currentLevelIndex);
    nextGameState = GAME_STATE_PLAYING_SCREEN;
    cameFromGame = false;
  }
  else if (!isKeyDown[SDL_SCANCODE_E])
  {
    ePressed = false;
  }

  // Handle save/load/new level

  if (isKeyDown[SDL_SCANCODE_S] && !sPressed)
  {
    sPressed = true;
    // Save current level first
    setLevel(currentLevelIndex, &editorLevel);

    if (saveLevelsToFile("src/assets/levels.dat"))
    {
      printf("Levels saved successfully!\n");
    }
  }
  else if (!isKeyDown[SDL_SCANCODE_S])
  {
    sPressed = false;
  }

  if (isKeyDown[SDL_SCANCODE_L] && !lPressed)
  {
    lPressed = true;
    if (loadLevelsFromFile("src/assets/levels.dat"))
    {
      currentLevelIndex = 0;
      if (getNumberOfLevels() > 0)
      {
        memcpy(&editorLevel, &getLevels()[0], sizeof(Level));
      }
      updateTextDisplays();
      printf("Levels loaded successfully!\n");
    }
  }
  else if (!isKeyDown[SDL_SCANCODE_L])
  {
    lPressed = false;
  }

  if (isKeyDown[SDL_SCANCODE_N] && !nPressed)
  {
    nPressed = true;
    // Save current level
    setLevel(currentLevelIndex, &editorLevel);

    // Create new blank level
    memset(&editorLevel, _, sizeof(Level));

    if (addLevel(&editorLevel))
    {
      currentLevelIndex = getNumberOfLevels() - 1;
      updateTextDisplays();
      printf("New level created!\n");
    }
  }
  else if (!isKeyDown[SDL_SCANCODE_N])
  {
    nPressed = false;
  }

  // Handle mouse drawing
  if (isMouseButtonDown[SDL_BUTTON_LEFT])
  {
    isDragging = true;

    int gridX, gridY;
    getGridPosition(motionX, motionY, &gridX, &gridY);

    int index = getPatternIndex(gridX, gridY);
    if (index >= 0)
    {
      editorLevel.pattern[index] = currentBrickType;
    }
  }
  else
  {
    isDragging = false;
  }

  // Right mouse button to erase
  if (isMouseButtonDown[SDL_BUTTON_RIGHT])
  {
    int gridX, gridY;
    getGridPosition(motionX, motionY, &gridX, &gridY);

    int index = getPatternIndex(gridX, gridY);
    if (index >= 0)
    {
      editorLevel.pattern[index] = _;
    }
  }
}

void drawEditorScreen(void)
{
  // Draw grid and bricks
  for (int y = 0; y < LEVEL_PATTERN_ROWS; y++)
  {
    for (int x = 0; x < LEVEL_PATTERN_COLS; x++)
    {
      int index = y * LEVEL_PATTERN_COLS + x;
      BrickType type = editorLevel.pattern[index];

      float posX = LEVEL_PATTERN_INDENT + x * (BRICK_WIDTH + LEVEL_PATTERN_SPACING);
      float posY = LEVEL_PATTERN_INDENT + y * (LEVEL_PATTERN_ROW_HEIGHT + LEVEL_PATTERN_SPACING) - editorCameraY;

      // Skip if off screen
      if (posY < -LEVEL_PATTERN_ROW_HEIGHT || posY > SCREEN_HEIGHT)
      {
        continue;
      }

      // Draw grid cell outline
      SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
      SDL_FRect gridRect = {posX, posY, BRICK_WIDTH, LEVEL_PATTERN_ROW_HEIGHT};
      SDL_RenderRect(renderer, &gridRect);

      // Draw brick if not empty
      if (type != _)
      {
        SDL_Color color;
        if (type == B)
        {
          color = (SDL_Color){255, 0, 0, 255};
        }
        else if (type == S)
        {
          color = (SDL_Color){255, 255, 255, 255};
        }
        else if (type == F)
        {
          color = (SDL_Color){0, 255, 0, 255};
        }
        else if (type == E)
        {
          color = (SDL_Color){255, 255, 0, 255};
        }
        else
        {
          color = (SDL_Color){128, 128, 128, 255};
        }

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_FRect brickRect = {posX + 1, posY + 1, BRICK_WIDTH - 2, LEVEL_PATTERN_ROW_HEIGHT - 2};
        SDL_RenderFillRect(renderer, &brickRect);
      }
    }
  }

  // Highlight current mouse position
  if (motionX >= 0 && motionY >= 0)
  {
    int gridX, gridY;
    getGridPosition(motionX, motionY, &gridX, &gridY);

    if (getPatternIndex(gridX, gridY) >= 0)
    {
      float posX = LEVEL_PATTERN_INDENT + gridX * (BRICK_WIDTH + LEVEL_PATTERN_SPACING);
      float posY = LEVEL_PATTERN_INDENT + gridY * (LEVEL_PATTERN_ROW_HEIGHT + LEVEL_PATTERN_SPACING) - editorCameraY;

      SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128);
      SDL_FRect highlightRect = {posX, posY, BRICK_WIDTH, LEVEL_PATTERN_ROW_HEIGHT};
      SDL_RenderRect(renderer, &highlightRect);
    }
  }

  // Draw UI text with background
  if (levelInfoText != NULL)
  {
    float w, h;
    SDL_GetTextureSize(levelInfoText, &w, &h);

    // Draw background
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 180);
    SDL_FRect bgRect = {5, 5, w + 10, h + 5};
    SDL_RenderFillRect(renderer, &bgRect);

    SDL_FRect dstRect = {10, 7, w, h};
    SDL_RenderTexture(renderer, levelInfoText, NULL, &dstRect);
  }

  if (brickTypeText != NULL)
  {
    float w, h;
    SDL_GetTextureSize(brickTypeText, &w, &h);

    // Draw background
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 180);
    SDL_FRect bgRect = {5, 30, w + 10, h + 5};
    SDL_RenderFillRect(renderer, &bgRect);

    SDL_FRect dstRect = {10, 32, w, h};
    SDL_RenderTexture(renderer, brickTypeText, NULL, &dstRect);
  }

  if (helpText != NULL)
  {
    float w, h;
    SDL_GetTextureSize(helpText, &w, &h);

    float w2 = w, h2 = h;
    if (helpText2 != NULL)
    {
      SDL_GetTextureSize(helpText2, &w2, &h2);
      if (w2 > w) w = w2;
    }

    // Draw background for both lines
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 180);
    SDL_FRect bgRect = {5, SCREEN_HEIGHT - h - h2 - 20, w + 10, h + h2 + 10};
    SDL_RenderFillRect(renderer, &bgRect);

    // First line
    SDL_FRect dstRect = {10, SCREEN_HEIGHT - h - h2 - 18, w, h};
    SDL_RenderTexture(renderer, helpText, NULL, &dstRect);

    // Second line
    if (helpText2 != NULL)
    {
      SDL_FRect dstRect2 = {10, SCREEN_HEIGHT - h2 - 15, w2, h2};
      SDL_RenderTexture(renderer, helpText2, NULL, &dstRect2);
    }
  }
}

void destroyEditorScreen(void)
{
  // Save current level before exiting
  setLevel(currentLevelIndex, &editorLevel);

  if (helpText != NULL)
  {
    SDL_DestroyTexture(helpText);
    helpText = NULL;
  }

  if (helpText2 != NULL)
  {
    SDL_DestroyTexture(helpText2);
    helpText2 = NULL;
  }

  if (levelInfoText != NULL)
  {
    SDL_DestroyTexture(levelInfoText);
    levelInfoText = NULL;
  }

  if (brickTypeText != NULL)
  {
    SDL_DestroyTexture(brickTypeText);
    brickTypeText = NULL;
  }
}
