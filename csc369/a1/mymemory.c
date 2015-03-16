#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h> 

#define MAGIC 0xfabdaded
#define SBRK_MULTI 4096

//2 types of structs: a linked list for free space, and a header allocated space
struct freelist {
  int size;
  struct freelist *next;
};

struct allocatedlist {
  int size;
  int magic;
};

struct freelist *free_head;
struct allocatedlist *allocated_head;

pthread_mutex_t free_lock;

/*Please note: I got rid of my helper functions because I was told that some
  people had trouble with locks and fixed that by getting rid of their helper
  functions. Apparently that was not the case for me! :( Currently the code
  works for all single threaded tests (even if you convert 5-parallel-allocs
  and 7-longradom to single threaded, it runs fine).*/


/* mymalloc_init: initializes data structures that malloc needs in
                  order to keep track of allocated and free blocks of 
                  memory.  Gets an initial chunk of memory for the heap from
                  the OS using sbrk() and marks it as free so that it can  be 
                  used in future calls to mymalloc()
                  Will return 1 on falure and 0 on success.
*/
int mymalloc_init() {

  void *new_mem_ptr;
  if ((new_mem_ptr = sbrk(SBRK_MULTI)) == (void*) -1)
  {
    return 1;
  }

  //Putting an initial page of heap memory into the freelist struct node,
  //free_head, and setting its size & next attributes
  free_head = new_mem_ptr;
  free_head->size = SBRK_MULTI;
  free_head->next = NULL;

  //Currently nothing is in the allocated list, so set it to NULL
  allocated_head = NULL;

  //Initializing the mutex lock here, before the pthreads are created
  pthread_mutex_init(&free_lock, NULL);  

  return 0; 
}


/* mymalloc: allocates memory on the heap of the requested size. The block
             of memory returned should always be padded so that it begins
             and ends on a word boundary.
     unsigned int size: the number of bytes to allocate.
     retval: a pointer to the block of memory allocated or NULL if the 
             memory could not be allocated. 
             (NOTE: the system also sets errno, but we are not the system, 
                    so you are not required to do so.)
*/
void *mymalloc(unsigned int size) {

  int avail_mem = 0;
  int full_size = sizeof(struct allocatedlist) + size;
  int freelist_size;


  //If the size the user tries to malloc plus the size of the allocatedlist 
  //header is less than the size of a freelist struct, will malloc at least the 
  //size of the freelist struct so that it can be later stored in freelist when 
  //freed. This includes mallocing for a size of 0: will still malloc at least 
  //the size of a freelist struct.
  if (full_size < sizeof(struct freelist))
  {
    full_size = sizeof(struct freelist);
  }

  //Checking if the size is a multiple of 8 -if not, round up
  //to make it be a multiple of 8
  int to_eight = full_size % 8;
  if (to_eight != 0)
  {
    full_size += (8-to_eight);
  }

  /*CRITICAL SECTION - locking until freelist is modified for the malloc call, 
  otherwise multiple threads may decide the same chunk in freelist can be used 
  for their malloc call*/
  pthread_mutex_lock(&free_lock);

  //Checking to see if there's available space in currently allocated heap
  struct freelist **freelist_ptr;
  for (freelist_ptr = &free_head; *freelist_ptr; freelist_ptr = 
    &((*freelist_ptr)->next))
  {
    
    //If it finds a match in size, break and use this chunk
    if ((*freelist_ptr)->size >= full_size)
    {
      avail_mem=1;
      break;
    }
  }

  //Only enter this block if there's no more memory available, need to call sbrk 
  //to get more 
  if (!avail_mem)
  {
    int multiples = (full_size / 4096) + 1;
    void *new_mem_ptr;
    if ((new_mem_ptr = sbrk(multiples * SBRK_MULTI)) == (void*) -1)
    {
      pthread_mutex_unlock(&free_lock);
      return NULL;
    }

    //Iterate to the end of the freelist to add the new node there, if there's 
    //anything in the freelist
    freelist_ptr = &free_head;

    if (*freelist_ptr)
    {
      for(; (*freelist_ptr)->next != NULL; freelist_ptr=&((*freelist_ptr)->next)) 
      {;
      }

      (*freelist_ptr)->next = new_mem_ptr;
      (*freelist_ptr)->next->next = NULL;

      //set its size
      (*freelist_ptr)->next->size = multiples * SBRK_MULTI;

      //Coalescing: No need to coalesce if freelist was empty prior to
      //adding this node

      //First do pointer arithmetic:
      char* temp_address = (char*) (*freelist_ptr);
      temp_address += (*freelist_ptr)->size; 


      //Check if coalescing is necessary - do so if it is
      if ((void*)temp_address == (*freelist_ptr)->next)
      {
          (*freelist_ptr)->size = (*freelist_ptr)->size 
            + (*freelist_ptr)->next->size;
          (*freelist_ptr)->next = (*freelist_ptr)->next->next;
      }
      else
      {
        *freelist_ptr = (*freelist_ptr)->next;
      }

    }

    //Case where freelist is empty - just add the node in
    else
    {
      *freelist_ptr = new_mem_ptr;
      (*freelist_ptr)->size = multiples * SBRK_MULTI;
      (*freelist_ptr)->next = NULL;
      free_head = *freelist_ptr; 
    }

  }


  //Now there is definitely available memory - the remainder of mymalloc will 
  //take it off the free list, put it onto the allocated list, and finally, 
  //return it

  //store the size of the chunk in a variable to re-assign it
  freelist_size = (*freelist_ptr)->size;
  
  //allocated_head will begin at the address in freelist_ptr
  allocated_head = (void*) *freelist_ptr;

  //remove the chunk at freelist_ptr from the free list:

  //CORNER CASE: if the remainder of the chunk is too small to store the header,
  //remove this part too and allocate it to the allocated_head list by increasing 
  //the size
  if ((freelist_size <= (full_size + sizeof(struct freelist)))) // || (freelist_size == full_size))
  {
    allocated_head->size = (*freelist_ptr)->size;

    //Iterating through list to find the prev pointer
    struct freelist *temp_list = free_head;
    struct freelist *temp_prev_ptr = NULL;

    for (; temp_list; temp_list = temp_list->next)
    { 
      if (temp_list == *freelist_ptr)
      {
        break;
      }
      else
      {
        temp_prev_ptr = temp_list;
      }
    }

    //if node is not found in the list, return 1
    if (temp_list == NULL)
    {
      printf("error - not in list");
      pthread_mutex_unlock(&free_lock);
      return NULL;
    }

    if (temp_prev_ptr != NULL)
    {
      temp_prev_ptr->next = temp_list->next;
    }
    else if (temp_list == free_head)
    {
      free_head = free_head->next;
    }
    else
    {
      free_head = NULL;
    }
  }

  //Otherwise there is enough space for the remainder of the header, so only
  //allocate a part of the chunk
  else
  {

    //Save the ->next in a temp variable since we'll need to set it again
    //once we perform pointer arithmetic
    struct freelist *temp_next = (*freelist_ptr)->next;

    //Do pointer arithmetic here:
    char *temp_address = (char*) *freelist_ptr;
    temp_address += full_size; 
    *freelist_ptr = (void*)temp_address;

    (*freelist_ptr)->size = freelist_size - full_size;
    (*freelist_ptr)->next = temp_next;

    //size includes any extra space allocated for header & other
    allocated_head->size = full_size;
  }

  /*END CRITICAL SECTION - Finished with the freelist linked list, now other 
  threads can modify it if they wish*/
  pthread_mutex_unlock(&free_lock);

  allocated_head->magic = MAGIC;

  //return the address after the header - cast to char* so increments by 1*size 
  //of struct
  return (void *)((char*) allocated_head + (sizeof(struct allocatedlist)));
}


/* myfree: unallocates memory that has been allocated with mymalloc.
     void *ptr: pointer to the first byte of a block of memory allocated by 
                mymalloc.
     retval: 0 if the memory was successfully freed and 1 otherwise.
             (NOTE: the system version of free returns no error.)
*/
unsigned int myfree(void *ptr) {

  //Finding the beginning of the allocated address, since ptr is the address 
  //starting after the header - store in *beginning_ptr
  struct allocatedlist *beginning_ptr;
  beginning_ptr = ptr;
  int size = sizeof(struct allocatedlist);

  //Doing pointer arithmetic:
  char* temp_address = (char*) beginning_ptr;
  temp_address -= size; 
  beginning_ptr = (void*)temp_address;

  //Integrity check to make sure the magic number is still there - if not,
  //free was unsuccessful - return 1
  if (beginning_ptr->magic != MAGIC)
  {
    return 1;
  }

  /*CRITICAL SECTION - Beginning to search through and alter freelist, which only
  one thread may do at a time*/
  pthread_mutex_lock(&free_lock);

  
  //Searching through freelist starting at free_head to find where to put a new 
  //node in the list so that it is in ascending memory order
  struct freelist *freelist_ptr = free_head;
  struct freelist *prev_ptr = NULL;
  for (;freelist_ptr; freelist_ptr=freelist_ptr->next)
  {
    if((char*)freelist_ptr > (char*)beginning_ptr)
    {
      break;
    }
    prev_ptr = freelist_ptr;
  }

  //putting the new node in the list - create a freelist struct for it, since it 
  //was previously an allocatedlist struct
  struct freelist *new_node = (void*)beginning_ptr;
  new_node->size = beginning_ptr->size;
  new_node->next = freelist_ptr;

  //If a freed element at a memory address before the current one exists, set 
  //the current one to its ->next, and check if you can coalesce the two
  if (prev_ptr)
  {
    prev_ptr->next = new_node;  
    

    //coalesce if a previous pointer exists here:

    //Pointer arithmetic to check previous address
    char* temp_prev = (char*) prev_ptr;
    temp_prev += prev_ptr->size; 
    
    if ((void*)temp_prev == new_node)
    {

        prev_ptr->size = prev_ptr->size + new_node->size;
        prev_ptr->next = new_node->next;
        new_node = prev_ptr;
    }
  }

  //if no smaller element exists on the freed list, the most recent freed 
  //element is put on the head
  else
  {
    free_head = new_node;
  }

  //coalesce if the new_node has something after it:

  //Pointer arithmetic to check next address
  char* temp_next = (char*) new_node;
  temp_next += new_node->size; 

  if ((void*)temp_next == new_node->next)
  {
    new_node->size = new_node->size + new_node->next->size;
    new_node->next = new_node->next->next;
  }

  /*FINISHED CRITICAL SECTION - done with modifying the freelist*/
  pthread_mutex_unlock(&free_lock);
  
  return 0;
}
