#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h> //do I need this? Apparently int sizes differ on diff. platforms and this prevents it for the mem address?
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define MAGIC 0xfabdaded
#define SBRK_MULTI 4096

//2 types of linkedlists: the one for free space, and the one for allocated space

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

void* ptr_arith(void *ptr, int additor);
struct freelist* coalesce(struct freelist *ptr1, struct freelist *ptr2);

sem_t sem;

//pthread_mutex_t free_lock; 
//= PTHREAD_MUTEX_INITIALIZER;

//static void *new_mem_ptr;

/* mymalloc_init: initializes data structures that malloc needs in
                  order to keep track of allocated and free blocks of 
                  memory.  Gets an initial chunk of memory for the heap from
                  the OS using sbrk() and marks it as free so that it can  be 
                  used in future calls to mymalloc()
                  Will return 1 on falure and 0 on success.
*/
int mymalloc_init() {

  sem_init(&sem, 0, 1);
  sem_wait(&sem);
  void *new_mem_ptr;
  if ((new_mem_ptr = sbrk(SBRK_MULTI)) == (void*) -1)
  {
    sem_post(&sem);
    return 1;
  }
  
  //pthread_mutex_lock(&free_lock);
//Pointing to the address at new_mem_ptr to store data
  free_head = new_mem_ptr;
  free_head->size = SBRK_MULTI;
  free_head->next = NULL;

  allocated_head = NULL;
  sem_post(&sem);
  //pthread_mutex_unlock(&free_lock);
  //pthread_mutex_init(&free_lock,  NULL);  
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

  sem_wait(&sem);

  int avail_mem = 0;
  int full_size = sizeof(struct allocatedlist) + size;
  int freelist_size;

  //If the size the user tries to malloc plus the size of the allocatedlist header 
  //is less than the size of a freelist struct, will malloc at least the size of the 
  //freelist struct so that it can be later stored in freelist. 
  //This includes mallocing for a size of 0: will still malloc at least the size of a freelist struct.
  if (full_size < sizeof(struct freelist))
  {
    full_size = sizeof(struct freelist);
  }

  //checking if the size is a multiple of 8 -if not, round up
  //to make it be a multiple of 8
  int to_eight = full_size % 8;
  if (to_eight != 0)
  {
    full_size += (8-to_eight);
  }

  /*CRITICAL SECTION - locking until freelist is modified for the malloc call, otherwise multiple
  threads may decide the same chunk in freelist can be used for their malloc call*/
  //pthread_mutex_lock(&free_lock);
  //sem_wait(&sem);

  //Checking to see if there's available space in currently allocated heap
  struct freelist **freelist_ptr;
  for (freelist_ptr = &free_head; *freelist_ptr; freelist_ptr = &((*freelist_ptr)->next))
  {
    //If it finds a match in size, break and use this chunk
    if ((*freelist_ptr)->size >= full_size)
    {
      avail_mem=1;
      break;
    }
  }


  //Only enter this block if there's no more memory available, need to call sbrk to get more 
  if (!avail_mem)
  {
    int multiples = (size / 4096) + 1;
    void *new_mem_ptr;
    if ((new_mem_ptr = sbrk(multiples * SBRK_MULTI)) == (void*) -1)
    {
      //pthread_mutex_unlock(&free_lock);
      sem_post(&sem);
      return NULL;
    }

    //Iterate to the end of the freelist to add the new node there  
    freelist_ptr = &free_head;

    if (*freelist_ptr)
    {

      for(; (*freelist_ptr)->next != NULL; freelist_ptr=&((*freelist_ptr)->next)) {
        ;}

      (*freelist_ptr)->next = new_mem_ptr;
      (*freelist_ptr)->next->next = NULL;

      //Coalescing: No need to coalesce if freelist was empty prior to
      //adding this node
      if (ptr_arith(*freelist_ptr, (*freelist_ptr)->size) == (*freelist_ptr)->next)
      {
        *freelist_ptr = coalesce(*freelist_ptr, (*freelist_ptr)->next);
      }

    }

    //Case where freelist is empty - just add the node in
    else
    {
      *freelist_ptr = new_mem_ptr;
      (*freelist_ptr)->next = NULL;
    }
    (*freelist_ptr)->size = multiples * SBRK_MULTI;

  }


  //Now there is definitely available memory - this chunk of code will take it off the free list, put
  //it onto the allocated list, and finally, return it

  //store the size of the chunk in a variable to re-assign it
  freelist_size = (*freelist_ptr)->size;

  //allocated_head will begin at the address in freelist_ptr
  allocated_head = (void*) *freelist_ptr;

  //remove the chunk at freelist_ptr from the free list:

  //CORNER CASE: if the remainder of the chunk is too
  //small to store the header, remove this part too and allocate it to the allocated_head list
  //by increasing the size

  printf("freelist_size is %d, full_size is %d and sizeof freelist is %d \n", freelist_size, full_size, (int)sizeof(struct freelist));

  if (freelist_size < (full_size + sizeof(struct freelist)))
  {
    printf("in corner case\n");
    //printf("freelist ptr size is %d, fullsize + sizeof struct is %d\n", (int)(*freelist_ptr)->size, (int)(full_size+sizeof(struct freelist)));
    allocated_head->size = (*freelist_ptr)->size;

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
      sem_post(&sem);
      return NULL;
    }

    if (temp_prev_ptr != NULL)
    {
      temp_prev_ptr->next = temp_list->next;
    }
    else
    {
      free_head = NULL;
    }
  }

  //Otherwise there is enough space for the remainder of the header, so only
  //allocate a specific chunk
  else
  {

    //Save the ->next in a temp variable since we'll need to set it again
    //once we perform pointer arithmetic
    struct freelist *temp_next = (*freelist_ptr)->next;

    *freelist_ptr = ptr_arith(*freelist_ptr, full_size);

    (*freelist_ptr)->size = freelist_size - full_size;
    (*freelist_ptr)->next = temp_next;

  //size includes any extra space allocated for header & other
    allocated_head->size = full_size;
  }
  
  allocated_head->magic = MAGIC;

  /*END CRITICAL SECTION - Finished with the freelist linked list, now other threads can modify it if they wish*/
  //pthread_mutex_unlock(&free_lock);
  sem_post(&sem);



  //return the address after the header - cast to char* so increments by 1*size of struct
  return (void *)((char*) allocated_head + (sizeof(struct allocatedlist)));


}


/* myfree: unallocates memory that has been allocated with mymalloc.
     void *ptr: pointer to the first byte of a block of memory allocated by 
                mymalloc.
     retval: 0 if the memory was successfully freed and 1 otherwise.
             (NOTE: the system version of free returns no error.)
*/
unsigned int myfree(void *ptr) {

  sem_wait(&sem);

  //finding the beginning of the allocated address, since ptr is the address starting after
  //the header - store in *beginning_ptr
  struct allocatedlist *beginning_ptr;
  beginning_ptr = ptr;
  int size = sizeof(struct allocatedlist);
  beginning_ptr = ptr_arith(beginning_ptr, -size);

  printf("freeing ptr at address %p\n", beginning_ptr);


  //integrity check to make sure the magic number is still there - if not,
  //free was unsuccessful - return 1
  if (beginning_ptr->magic != MAGIC)
  {
    return 1;
  }


  /*CRITICAL SECTION - Beginning to search through and alter freelist, which only
  one thread may do at a time*/

  //pthread_mutex_lock(&free_lock);
  //sem_wait(&sem);

  //Searching through freelist starting at free_head to find where to put a new node in the list
  //so that it is in ascending memory order
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



  //putting the new node in the list - create a freelist struct 
  //for it, since it was previously an allocatedlist struct
  struct freelist *new_node = (void*)beginning_ptr;
  new_node->size = beginning_ptr->size;
  new_node->next = freelist_ptr;

  //If a freed element at a memory address before the current one
  //exists, set the current one to its ->next, and check if you can
  //coalesce the two
  if (prev_ptr)
  {
    prev_ptr->next = new_node;  
    //coalesce if a previous pointer exists here:
          //CALL PTR ARITH HERE I THINK.
    if (ptr_arith(prev_ptr, prev_ptr->size) == new_node)
    {
      new_node = coalesce(prev_ptr, new_node);
    }
  }

  //if no smaller element exists on the freed list, the most recent
  //freed element is put on the head
  else
  {
    free_head = new_node;
  }

  //coalesce if the new_node has something after it

  if (ptr_arith(new_node, new_node->size) == new_node->next)
  {
    coalesce(new_node, new_node->next);
  }

  /*FINISHED CRITICAL SECTION - done with modifying the freelist*/
  //pthread_mutex_unlock(&free_lock);
  sem_post(&sem);
  /*
  struct freelist *tempp = free_head;
  for (; tempp; tempp = tempp->next)
  {
    printf("freelist mem is: %p and size is: %d\n", tempp, tempp->size);
  }
*/
  return 0;
}



/* 
ptr_arith adds additor to ptr and returns the resulting address
*/
void* ptr_arith(void *ptr, int additor) {
  char* temp_address = (char*) ptr;
  temp_address += additor; 
  ptr = (void*)temp_address;
  return ptr;
}

struct freelist *coalesce(struct freelist *ptr1, struct freelist *ptr2)
{
  //Set pointer 1 to be the new coalesced pointer
  ptr1->size = ptr1->size + ptr2->size;
  ptr1->next = ptr2->next;
  return ptr1;
}