#include "level-manager.h"
#include "globals.h"
#include "embedded_assets.h"
#include <string.h>
#include <ctype.h>

#define MAX_LEVELS 100
#define LEVEL_FILE_PREFIX "assets/levels/level"
#define LEVEL_FILE_SUFFIX ".txt"

static Level levels[MAX_LEVELS];
static int numLevels = 0;

// What getLevel() hands back for an index it has not got. Filled in by
// initializeLevelManager(), so it is a valid empty level rather than zeroes:
// an all-'_' pattern builds no bricks and no enemies, and the name is what the
// HUD and the level banner print.
static Level fallbackLevel;

const Level *getLevel(int index)
{
  if (index < 0 || index >= numLevels)
  {
    return &fallbackLevel;
  }

  return &levels[index];
}

int getNumberOfLevels(void)
{
  return numLevels;
}

static bool isValidCell(char c)
{
  return c == 'B' || c == 'D' || c == 'T' || c == 'S' || c == 'X' ||
         c == 'G' || c == 'F' || c == 'E' || c == 'V' || c == 'W' || c == 'U';
}

// Load one level from a text buffer (embedded or file data).
// A line starting with '#' sets the level name; other cells are pattern chars.
static bool parseLevelFromText(Level *level, const char *text, size_t size)
{
  memset(level->pattern, '_', sizeof(level->pattern));
  level->name[0] = '\0';

  int row = 0;
  size_t i = 0;

  while (i < size && row < LEVEL_PATTERN_ROWS)
  {
    // Name / comment line
    if (text[i] == '#')
    {
      i++;
      while (i < size && (text[i] == ' ' || text[i] == '\t'))
      {
        i++;
      }

      bool firstNameLine = level->name[0] == '\0';
      int len = 0;
      while (i < size && text[i] != '\n' && text[i] != '\r')
      {
        if (firstNameLine && len < LEVEL_NAME_MAX - 1)
        {
          level->name[len++] = text[i];
        }
        i++;
      }
      if (firstNameLine)
      {
        level->name[len] = '\0';
      }
    }
    else
    {
      // Pattern line
      int col = 0;
      while (i < size && text[i] != '\n' && text[i] != '\r')
      {
        if (col < LEVEL_PATTERN_COLS)
        {
          char c = text[i];
          level->pattern[row * LEVEL_PATTERN_COLS + col] = isValidCell(c) ? c : '_';
          col++;
        }
        i++;
      }
      row++;
    }

    // Skip newline(s)
    while (i < size && (text[i] == '\n' || text[i] == '\r'))
    {
      i++;
    }
  }

  if (level->name[0] == '\0')
  {
    snprintf(level->name, LEVEL_NAME_MAX, "LEVEL");
  }

  return true;
}

// Load a single level txt file by 1-based index
static bool loadLevelFile(int index)
{
  char filename[256];
  snprintf(filename, sizeof(filename), "%s%02d%s", LEVEL_FILE_PREFIX, index, LEVEL_FILE_SUFFIX);

  const EmbeddedAsset *asset = findEmbeddedAsset(filename);

  if (asset != NULL)
  {
    return parseLevelFromText(&levels[numLevels], (const char *)asset->data, asset->size);
  }

  // Try filesystem fallback
  SDL_IOStream *file = SDL_IOFromFile(filename, "r");
  if (file == NULL)
  {
    return false;
  }

  Sint64 fileSize = SDL_GetIOSize(file);
  if (fileSize <= 0)
  {
    SDL_CloseIO(file);
    return false;
  }

  char *buf = (char *)SDL_malloc((size_t)fileSize + 1);
  if (buf == NULL)
  {
    SDL_CloseIO(file);
    return false;
  }

  size_t bytesRead = SDL_ReadIO(file, buf, (size_t)fileSize);
  SDL_CloseIO(file);
  buf[bytesRead] = '\0';

  bool ok = parseLevelFromText(&levels[numLevels], buf, bytesRead);
  SDL_free(buf);
  return ok;
}

int getLevelContentBottom(int levelIndex)
{
  if (levelIndex < 0 || levelIndex >= numLevels)
  {
    return LEVEL_PATTERN_TOP + LEVEL_PATTERN_ROWS * (LEVEL_PATTERN_ROW_HEIGHT + LEVEL_PATTERN_SPACING);
  }

  const Level *level = &levels[levelIndex];

  int lastRow = -1;
  for (int row = LEVEL_PATTERN_ROWS - 1; row >= 0; row--)
  {
    for (int col = 0; col < LEVEL_PATTERN_COLS; col++)
    {
      if (level->pattern[row * LEVEL_PATTERN_COLS + col] != '_')
      {
        lastRow = row;
        break;
      }
    }
    if (lastRow >= 0)
    {
      break;
    }
  }

  if (lastRow < 0)
  {
    return SCREEN_HEIGHT / 2;
  }

  return LEVEL_PATTERN_TOP + (lastRow + 1) * (LEVEL_PATTERN_ROW_HEIGHT + LEVEL_PATTERN_SPACING);
}

void initializeLevelManager(void)
{
  numLevels = 0;

  memset(fallbackLevel.pattern, '_', sizeof(fallbackLevel.pattern));
  snprintf(fallbackLevel.name, LEVEL_NAME_MAX, "LEVEL");

  for (int i = 1; i <= MAX_LEVELS; i++)
  {
    if (!loadLevelFile(i))
    {
      break;
    }
    numLevels++;
  }

  if (numLevels == 0)
  {
    fprintf(stderr, "Warning: no level files found.\n");
  }
  else
  {
    printf("Loaded %d levels.\n", numLevels);
  }
}

void destroyLevelManager(void)
{
  numLevels = 0;
}
