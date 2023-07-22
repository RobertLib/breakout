#include "enemies.h"
#include "levels.h"

int numEnemies;

Enemy *enemies;

void initializeEnemies(void)
{
  numEnemies = 0;

  for (int i = 0; i < LEVEL_PATTERN_LENGTH; i++)
  {
    if (LEVELS[paddle.level].pattern[i] == E)
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
    if (LEVELS[paddle.level].pattern[i] == E)
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

  enemies = NULL;

  numEnemies = 0;
}
