#include "list.h"

// Create a new list with initialization to NULL using calloc
List *newList(void)
{
  List *list = (List *)calloc(1, sizeof(List));

  if (list == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for the list.\n");
    return NULL;
  }

  return list;
}

// Add a new item to the list
void listAdd(List *list, void *data)
{
  if (list == NULL)
  {
    fprintf(stderr, "Provided list is NULL.\n");
    return;
  }

  Node *newNode = (Node *)malloc(sizeof(Node));

  if (newNode == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for newNode.\n");
    return;
  }

  newNode->data = data;
  newNode->next = NULL;

  if (list->tail == NULL)
  {
    list->head = newNode;
    list->tail = newNode;
  }
  else
  {
    list->tail->next = newNode;
    list->tail = newNode;
  }
}

// Free a single node
static void freeNode(Node *node)
{
  if (node)
  {
    // free(node->data);
    free(node);
    node = NULL;
  }
}

// Free the entire list
void freeList(List *list)
{
  if (list)
  {
    Node *current = list->head;
    Node *next = NULL;

    while (current != NULL)
    {
      next = current->next;
      freeNode(current);
      current = next;
    }

    free(list);
    list = NULL;
  }
}
