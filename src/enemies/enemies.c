#include "enemies.h"
#include "../level-types.h"
#include "../level-manager.h"

int numEnemies;

Enemy *enemies;

SDL_Texture *enemyTexture;

void initializeEnemies(void)
{
  numEnemies = 0;

  Level *currentLevel = &getLevels()[paddle.level];

  for (int i = 0; i < LEVEL_PATTERN_LENGTH; i++)
  {
    if (currentLevel->pattern[i] == E)
    {
      numEnemies++;
    }
  }

  enemies = (Enemy *)malloc(sizeof(Enemy) * numEnemies);

  if (enemies == NULL)
  {
    fprintf(stderr, "Memory not allocated.\n");
    exit(1);
  }

  int enemyIndex = 0;
  int x = 0;
  int y = 0;

  // Create enemies based on the level pattern
  for (int i = 0; i < LEVEL_PATTERN_LENGTH; i++)
  {
    if (currentLevel->pattern[i] == E)
    {
      enemyIndex++;

      initializeEnemy(
          &enemies[enemyIndex - 1],
          LEVEL_PATTERN_INDENT + x * (BRICK_WIDTH + LEVEL_PATTERN_SPACING),
          LEVEL_PATTERN_INDENT + y * (BRICK_HEIGHT + LEVEL_PATTERN_SPACING));
    }

    x++;

    if (x >= LEVEL_PATTERN_COLS)
    {
      x = 0;
      y++;
    }
  }

  enemyTexture = loadTexture("src/enemies/enemy.bmp");
}

void updateEnemies(void)
{
  for (int i = 0; i < numEnemies; i++)
  {
    updateEnemy(&enemies[i]);
  }
}

void drawEnemies(void)
{
  for (int i = 0; i < numEnemies; i++)
  {
    drawEnemy(&enemies[i]);
  }
}

void destroyEnemies(void)
{
  free(enemies);

  SDL_DestroyTexture(enemyTexture);

  enemies = NULL;
  enemyTexture = NULL;

  numEnemies = 0;
}
