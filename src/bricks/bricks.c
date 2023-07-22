#include "bricks.h"
#include "../level-types.h"
#include "../level-manager.h"
#include "../paddle/paddle.h"
#include "../utils/list.h"

#define CELL_WIDTH 60
#define CELL_HEIGHT 30
#define GRID_WIDTH (800 / CELL_WIDTH)
#define GRID_HEIGHT (LEVEL_HEIGHT / CELL_HEIGHT)

int numBricks;

Brick *bricks;

SDL_Texture *bricksTexture;
SDL_Texture *brickItemsTexture;

static List *grid[GRID_WIDTH][GRID_HEIGHT];

static void assignBricksToGrid(void)
{
  for (int i = 0; i < numBricks; i++)
  {
    int gridX = bricks[i].pos.x / CELL_WIDTH;
    int gridY = bricks[i].pos.y / CELL_HEIGHT;

    // Check if gridX and gridY are within the valid range before accessing the array
    if (gridX < 0 || gridX >= GRID_WIDTH || gridY < 0 || gridY >= GRID_HEIGHT)
    {
      fprintf(stderr, "Brick position out of grid bounds: (%d, %d)\n", gridX, gridY);
      continue; // Skip this iteration to avoid accessing out-of-bounds index
    }

    if (!grid[gridX][gridY])
    {
      grid[gridX][gridY] = newList();

      if (!grid[gridX][gridY])
      {
        fprintf(stderr, "Failed to create new list for grid cell (%d, %d).\n", gridX, gridY);
        continue;
      }
    }

    listAdd(grid[gridX][gridY], &bricks[i]);
  }
}

void checkSurroundingTilesInGrid(
    Obj *obj,
    SurroundingTilesCallback surroundingTilesCallback)
{
  int gridX = obj->pos.x / CELL_WIDTH;
  int gridY = obj->pos.y / CELL_HEIGHT;

  // Check the 8 surrounding cells
  for (int dx = -1; dx <= 1; dx++)
  {
    for (int dy = -1; dy <= 1; dy++)
    {
      int checkX = gridX + dx;
      int checkY = gridY + dy;

      if (checkX >= 0 &&
          checkX < GRID_WIDTH &&
          checkY >= 0 &&
          checkY < GRID_HEIGHT)
      {
        List *bricksList = grid[checkX][checkY];

        if (bricksList != NULL)
        {
          for (Node *node = bricksList->head; node != NULL; node = node->next)
          {
            Brick *brick = (Brick *)node->data;

            if (brick->active)
            {
              surroundingTilesCallback(obj, brick);
            }
          }
        }
      }
    }
  }
}

bool areAllFinalBricksInactive(void)
{
  bool allInactive = true;

  for (int i = 0; i < numBricks; i++)
  {
    if (bricks[i].type == F && bricks[i].active)
    {
      allInactive = false;
      break;
    }
  }

  return allInactive;
}

void initializeBricks(void)
{
  numBricks = 0;

  // Clear the grid first
  for (int i = 0; i < GRID_WIDTH; i++)
  {
    for (int j = 0; j < GRID_HEIGHT; j++)
    {
      grid[i][j] = NULL;
    }
  }

  Level *currentLevel = &getLevels()[paddle.level];

  for (int i = 0; i < LEVEL_PATTERN_LENGTH; i++)
  {
    if (
        currentLevel->pattern[i] != _ &&
        currentLevel->pattern[i] != E)
    {
      numBricks++;
    }
  }

  bricks = (Brick *)malloc(sizeof(Brick) * numBricks);

  if (bricks == NULL)
  {
    fprintf(stderr, "Memory not allocated.\n");
    exit(1);
  }

  int brickIndex = 0;
  int x = 0;
  int y = 0;

  // Create bricks based on the level pattern
  for (int i = 0; i < LEVEL_PATTERN_LENGTH; i++)
  {
    BrickType type = currentLevel->pattern[i];

    if (type != _ && type != E)
    {
      brickIndex++;

      initializeBrick(
          &bricks[brickIndex - 1],
          LEVEL_PATTERN_INDENT + x * (BRICK_WIDTH + LEVEL_PATTERN_SPACING),
          LEVEL_PATTERN_INDENT + y * (BRICK_HEIGHT + LEVEL_PATTERN_SPACING),
          type);
    }

    x++;

    if (x >= LEVEL_PATTERN_COLS)
    {
      x = 0;
      y++;
    }
  }

  assignBricksToGrid();

  bricksTexture = loadTexture("src/bricks/bricks.bmp");
  brickItemsTexture = loadTexture("src/bricks/brick-items.bmp");
}

void updateBricks(void)
{
  for (int i = 0; i < numBricks; i++)
  {
    updateBrick(&bricks[i]);
  }
}

void drawBricks(void)
{
  for (int i = 0; i < numBricks; i++)
  {
    drawBrick(&bricks[i]);
  }
}

static void freeGrid(List *grid[][GRID_HEIGHT], int gridWidth, int gridHeight)
{
  for (int i = 0; i < gridWidth; i++)
  {
    for (int j = 0; j < gridHeight; j++)
    {
      if (grid[i][j] != NULL)
      {
        freeList(grid[i][j]);

        grid[i][j] = NULL;
      }
    }
  }
}

void destroyBricks(void)
{
  for (int i = 0; i < numBricks; i++)
  {
    destroyBrick(&bricks[i]);
  }

  free(bricks);

  bricks = NULL;

  numBricks = 0;

  freeGrid(grid, GRID_WIDTH, GRID_HEIGHT);

  SDL_DestroyTexture(bricksTexture);
  SDL_DestroyTexture(brickItemsTexture);

  bricksTexture = NULL;
  brickItemsTexture = NULL;
}
