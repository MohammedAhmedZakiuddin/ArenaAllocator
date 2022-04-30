#include "mavalloc.h"
#include "tinytest.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

enum TYPE // Tracks which linked list is used or not
{
  FREE = 0,
  USED = 1,
};

// Creating The linked list structure
typedef struct Node
{

  int size;
  enum TYPE type;     // 1 is used and 0 is freed
  void *arenaPointer; // Points to the address of the arena
  struct Node *next;
  struct Node *prev;

} Node;

// Declaring the linked list
Node *LinkedList;
// Declaring and initializing the arena that allocate memory from
void *Arena = NULL;

// Declaring a previous node to easily access it within the algorithms given
Node *previous_node;

// Declarating and initializing an allocation algorithm
enum ALGORITHM allocation_algorithm = FIRST_FIT;


/*
This function will use malloc to allocate a pool of memory that is size bytes big. If 
the size parameter is less than zero then return -1.  If allocation fails return -1.  If the 
allocation succeeds return 0
*/
int mavalloc_init(size_t size, enum ALGORITHM algorithm)
{

  allocation_algorithm = algorithm;
  // checks if the size is valid
  if (size < 0)
  {
    return -1;
  }
  // Size must be 4 byte aligned
  Arena = malloc(ALIGN4(size));

  // Checks arena allocation is valid
  if (Arena == NULL)
  {
    return -1;
  }

  // Allocating a node for the linked list
  LinkedList = (Node *)malloc(sizeof(Node));

  // Initializing Values to the linked list attributes
  LinkedList->type = FREE;
  LinkedList->size = ALIGN4(size); // making the size four bytes aligned
  LinkedList->arenaPointer = Arena;
  LinkedList->next = NULL;
  LinkedList->prev = NULL;

  previous_node = LinkedList; // Keeping track of the previous node to be able to check if there existed values previously

  // if alloctation doesn't work
  if (LinkedList == NULL)
  {
    return -1;
  }

  // if allocation succeeds
  return 0;
}


//This function will free the allocated arena and empty the linked list
void mavalloc_destroy()
{
  free(Arena); // freeing the arena allocater
  Node *head = LinkedList;

  while (head) // Going through the linked list and freeing all nodes
  {
    struct Node *ptr = head;
    head = head->next;
    free(ptr);
  }
  LinkedList = NULL; // initialize the linked list to NULL to avoid any memory leaks
}

/*
This function will allocate size bytes from your preallocated memory arena using the 
heap allocation algorithm that was specified during mavalloc_init.  This function 
returns a pointer to the memory on success and NULL on failure. 
*/
void *mavalloc_alloc(size_t size)
{
  struct Node *node;

  if (allocation_algorithm != NEXT_FIT) 
  {
    node = LinkedList; // Setting the node equal to the head of the list
  }
  else if (allocation_algorithm == NEXT_FIT) 
  {
    node = previous_node; // To keep track of the linked list size when previous nodes are filled
  }
  else
  {
    printf("ERROR: Unknown allocation algorithm!\n");
    exit(0);
  }

  size_t aligned_size = ALIGN4(size); // making size aligned by 4 bytes

  // Executing the FIRST FIT algorithm
  if (allocation_algorithm == FIRST_FIT)
  {
    while (node) // While at tail of linked list
    {
      if (node->size >= aligned_size && node->type == FREE) // Checking the conditions for first fit
      {
        int leftover_size = 0; // keeping track of the remaining size to check validity

        node->type = USED; // Occupy the node
        leftover_size = node->size - aligned_size; 
        node->size = aligned_size; // Allocate space a block in the linkedlist

        // Checking if the leftover size is valid to allocate from the arena
        if (leftover_size > 0)
        {
          Node *previous_next = node->next;
          Node *leftover_node = (struct Node *)malloc(sizeof(struct Node));

          leftover_node->arenaPointer = node->arenaPointer + size; // Updating arena size
          leftover_node->type = FREE;
          leftover_node->size = leftover_size;
          leftover_node->next = previous_next;

          node->next = leftover_node;
        }
        node->prev = node;

        return (void *)node->arenaPointer; // arena allocater
      }
      node = node->next;
    }
  }
  
  // Executing the NEXT FIT algorithm
  else if (allocation_algorithm == NEXT_FIT)
  {
    // Using the previous node to go back to the start
    node = previous_node;

    while (node)
    {
      if (node->size >= aligned_size && node->type == FREE) // Checking the conditions for Next Fit
      {
        int leftover_size = 0;

        node->type = USED; // Occupy the node
        leftover_size = node->size - aligned_size;
        node->size = aligned_size; // Allocate space a block in the linkedlist

        // Checking if the leftover size is valid to allocate from the arena
        if (leftover_size > 0)
        {
          struct Node *previous_next = node->next;
          struct Node *leftover_node = (struct Node *)malloc(sizeof(struct Node));

          leftover_node->arenaPointer = node->arenaPointer + size; // Updating arena size
          leftover_node->type = FREE;
          leftover_node->size = leftover_size;
          leftover_node->next = previous_next;

          node->next = leftover_node;
        }
        previous_node = node;
        return (void *)node->arenaPointer; // arena allocater
      }
      node = node->next;

      if (node == previous_node) // if we get back to where we started
      {
        break;
      }
      if (node == NULL) // go back to head
      {
        node = LinkedList;
      }
    }
  }

  // Executing the WORST FIT algorithm
  else if (allocation_algorithm == WORST_FIT)
  {
  int bestWinningSize = INT_MIN; // Setting a value to the lowest integer to compare it with the size for validity
  Node *winner_node = NULL; // The node that satisfies worst fit conditions

  while (node)
  {
    if (node->type == FREE && (int)((node->size) - size) >= 0 && (int)((node->size) - size) > bestWinningSize) // Checking the conditions for Worst Fit
    {
      winner_node = node; // Worst node that fits
      bestWinningSize = node->size - size;
    }
    node = node->next;
  }
  if (winner_node)
  {
    int leftover_size = 0;

    bestWinningSize = winner_node->size - size;

    winner_node->type = USED; // Occupy the node
    leftover_size = winner_node->size - aligned_size;
    winner_node->size = aligned_size; // Allocate space a block in the linkedlist

    // Checking if the leftover size is valid to allocate from the arena
    if (leftover_size > 0)
    {
      Node *previous_next = winner_node->next;
      Node *leftover_node = (struct Node *)malloc(sizeof(struct Node));

      leftover_node->arenaPointer = winner_node->arenaPointer + size;
      leftover_node->type = FREE;
      leftover_node->size = leftover_size;
      leftover_node->next = previous_next;

      winner_node->next = leftover_node;
    }
    return (void *)winner_node->arenaPointer;  // arena allocater
  }
}

else if (allocation_algorithm == BEST_FIT)
{
  int bestWinningSize = INT_MAX; // Setting a value to the highest integer to compare it with the size for validity
  Node *winner_node = NULL; // The node that satisfies best fit conditions

  while (node)
  {
    if (node->type == FREE && (int)((node->size) - size) >= 0 && (int)((node->size) - size) < bestWinningSize) // Checking the conditions for Best Fit
    {
      winner_node = node; // Best node that fits
      bestWinningSize = node->size - size;
    }
    node = node->next;
  }
  if (winner_node)
  {
    int leftover_size = 0;

    bestWinningSize = winner_node->size - size;

    winner_node->type = USED; // Occupy the node
    leftover_size = winner_node->size - aligned_size;
    winner_node->size = aligned_size; // Allocate space a block in the linkedlist
    
    // Checking if the leftover size is valid to allocate from the arena
    if (leftover_size > 0)
    {
      Node *previous_next = winner_node->next;
      Node *leftover_node = (struct Node *)malloc(sizeof(struct Node));

      leftover_node->arenaPointer = winner_node->arenaPointer + size;
      leftover_node->type = FREE;
      leftover_node->size = leftover_size;
      leftover_node->next = previous_next;

      winner_node->next = leftover_node;
    }
    return (void *)winner_node->arenaPointer; // arena allocater
  }
}

// only return NULL on failure
return NULL;
}

/*
This function will free the block pointed to by the pointer back to your preallocated 
memory arena.  This function returns no value.  If there are two consecutive blocks 
free then combine (coalesce) them. 
*/
void mavalloc_free(void *ptr)
{
  struct Node *node = LinkedList;
  while (node)
  {
    if (node->arenaPointer == ptr) // if the arena pointer is passed
    {
      if (node->type == FREE) // if it was already a hole (FREE)
      {
        printf("Warning: Double free detected\n");
      }

      node->type = FREE; // Set the pointer to a hole

      break;
    }
    node = node->next;
  }

  node = LinkedList;

  while (node)
  {
    if (node->next && node->type == FREE && node->next->type == FREE) // Looking for holes (FREE) in the linked list to skip over
    {
      struct Node *previous = node->next;
      node->size = node->size + node->next->size; // Setting the size of the node to include the next node if it was a hole (FREE)
      node->next = node->next->next; // Skipping the (FREE) hole node and moving to the next one
      free(previous); // Freeing the (FREE) hole node
      continue;
    }
    node = node->next;
  }

  return;
}

// This function will return the number of nodes in the memory area
int mavalloc_size()
{
  Node *temp = LinkedList;
  int number_of_nodes = 0; // initializing a counter for the number of nodes in the linked list
  while (temp != NULL) // while not at the end of the list
  {
    temp = temp->next;
    number_of_nodes++;
  }
  return number_of_nodes; 
}
