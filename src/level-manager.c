#include "level-manager.h"
#include "globals.h"
#include "embedded_assets.h"
#include <string.h>

#define MAX_LEVELS 100
#define LEVELS_FILE "src/assets/levels.dat"

static Level levels[MAX_LEVELS];
static int numLevels = 0;

Level* getLevels(void)
{
  return levels;
}

int getNumberOfLevels(void)
{
  return numLevels;
}

void setLevel(int index, const Level *level)
{
  if (index >= 0 && index < MAX_LEVELS)
  {
    memcpy(&levels[index], level, sizeof(Level));
    if (index >= numLevels)
    {
      numLevels = index + 1;
    }
  }
}

bool addLevel(const Level *level)
{
  if (numLevels >= MAX_LEVELS)
  {
    fprintf(stderr, "Cannot add level: maximum number of levels reached.\n");
    return false;
  }

  memcpy(&levels[numLevels], level, sizeof(Level));
  numLevels++;
  return true;
}

bool loadLevelsFromFile(const char *filename)
{
  SDL_IOStream *file = NULL;
  const EmbeddedAsset *asset = findEmbeddedAsset(filename);

  if (asset != NULL)
  {
    // Load from embedded assets
    file = SDL_IOFromConstMem(asset->data, asset->size);
  }
  else
  {
    // Try to load from file system
    file = SDL_IOFromFile(filename, "rb");
  }

  if (file == NULL)
  {
    fprintf(stderr, "Could not open levels file: %s\n", filename);
    return false;
  }

  // Read number of levels
  int loadedNumLevels;
  if (SDL_ReadIO(file, &loadedNumLevels, sizeof(int)) != sizeof(int))
  {
    fprintf(stderr, "Error reading number of levels\n");
    SDL_CloseIO(file);
    return false;
  }

  if (loadedNumLevels > MAX_LEVELS)
  {
    fprintf(stderr, "Too many levels in file: %d (max: %d)\n", loadedNumLevels, MAX_LEVELS);
    SDL_CloseIO(file);
    return false;
  }

  // Read all levels
  size_t bytesToRead = loadedNumLevels * sizeof(Level);
  if (SDL_ReadIO(file, levels, bytesToRead) != bytesToRead)
  {
    fprintf(stderr, "Error reading levels data\n");
    SDL_CloseIO(file);
    return false;
  }

  numLevels = loadedNumLevels;
  SDL_CloseIO(file);

  printf("Loaded %d levels from %s\n", numLevels, filename);
  return true;
}

bool saveLevelsToFile(const char *filename)
{
  SDL_IOStream *file = SDL_IOFromFile(filename, "wb");

  if (file == NULL)
  {
    fprintf(stderr, "Could not create levels file: %s\n", filename);
    return false;
  }

  // Write number of levels
  if (SDL_WriteIO(file, &numLevels, sizeof(int)) != sizeof(int))
  {
    fprintf(stderr, "Error writing number of levels\n");
    SDL_CloseIO(file);
    return false;
  }

  // Write all levels
  size_t bytesToWrite = numLevels * sizeof(Level);
  if (SDL_WriteIO(file, levels, bytesToWrite) != bytesToWrite)
  {
    fprintf(stderr, "Error writing levels data\n");
    SDL_CloseIO(file);
    return false;
  }

  SDL_CloseIO(file);

  printf("Saved %d levels to %s\n", numLevels, filename);
  return true;
}

static bool createEmptyLevelsFile(const char *filename)
{
  FILE *file = fopen(filename, "wb");
  if (!file)
  {
    fprintf(stderr, "Failed to create levels file: %s\n", filename);
    return false;
  }

  // Write 0 levels
  uint32_t zero = 0;
  fwrite(&zero, sizeof(uint32_t), 1, file);
  fclose(file);

  printf("Created empty levels file: %s\n", filename);
  return true;
}

void initializeLevelManager(void)
{
  // Try to load from file first
  if (!loadLevelsFromFile(LEVELS_FILE))
  {
    // If loading fails, create an empty file
    fprintf(stderr, "Levels file not found. Creating empty levels file...\n");

    if (!createEmptyLevelsFile(LEVELS_FILE))
    {
      fprintf(stderr, "ERROR: Could not create levels file!\n");
    }
    numLevels = 0;
  }
}

void destroyLevelManager(void)
{
  numLevels = 0;
}
