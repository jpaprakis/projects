#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "mymemory.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/* global list of free memory */
struct free_mem * free_list;

/* mymalloc_init: initialize any data structures that your malloc needs in
                  order to keep track of allocated and free blocks of 
                  memory.  Get an initial chunk of memory for the heap from
                  the OS using sbrk() and mark it as free so that it can  be 
                  used in future calls to mymalloc()
*/

int mymalloc_init() {
  struct free_mem * head;
  pthread_mutex_lock(&lock);
  if ((head = sbrk(PAGE)) == (void *) -1){
     pthread_mutex_unlock(&lock);
     return 1; // non-zero return value indicates an error
  }
  else{
    head->size = PAGE- sizeof(struct free_mem);
    head->next = NULL;
    free_list = head;
    pthread_mutex_unlock(&lock);
    return 0; 
  }
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
  pthread_mutex_lock(&lock);

  /* adjusting size to meet 8-byte alignment requirement */
  int buffer = size % ALIGNMENT;
  int reqSize = size + (ALIGNMENT - buffer);

  /* setting up pointers to traverse the free list*/
  struct free_mem * current = free_list;
  struct free_mem * previous = free_list;

  /* checking if the 1st free node has enough memory to meet required size */
  if(current->size > (reqSize + sizeof(struct header))){
    /*setting previous to NULL as a flag for recognizing that 
      the 1st node has enough memory */
    previous = NULL; 
  }
  else{
    /* finding the 1st free chunk of memory that is more than size required */
    current = current->next;
    while((current != NULL) && 
                        (current->size <= (reqSize + sizeof(struct header)))){
      current = current->next;
      previous = previous->next;
    }
    
    /* in case there is no free node to fit the required size */
    if ((previous->size <= (reqSize + sizeof(struct header))) && 
                                                          (current == NULL)){
      /* creating a new free node and adding to the end of free list*/
      struct free_mem * head;
      if((head = sbrk(PAGE)) == (void *)-1){ 
        pthread_mutex_unlock(&lock);
        return NULL;
      }
      head->size = PAGE - sizeof(struct free_mem);
      head->next = NULL;
      current = head;
    }
  }
  
  /* moving the free node to make space for (size + header size)
     and updating the size left in free memory block along with
     the current->next to point to new address of free node */
  const void * src = (void *)current;
  void * free_node_dest = (void *)((char*)src + reqSize);
  
  /* if the free node is the first node in the free list*/
  if (previous == NULL){
    free_list = memmove(free_node_dest, src, sizeof(struct free_mem));
    free_list->size = free_list->size - reqSize;
  }else{
    previous->next = memmove(free_node_dest, src, sizeof(struct free_mem));
    previous->next->size = previous->next->size - reqSize;
  }

  /* initializing header for allocated block */
  struct header * new_head = (struct header *)src;
  new_head->size = size;
  new_head->magic = MAGICNUMBER;

  pthread_mutex_unlock(&lock);

  return (void *)((char*)new_head + sizeof(struct header));
}


/* myfree: unallocates memory that has been allocated with mymalloc.
     void *ptr: pointer to the first byte of a block of memory allocated by 
                mymalloc.
     retval: 0 if the memory was successfully freed and 1 otherwise.
             (NOTE: the system version of free returns no error.)
*/
unsigned int myfree(void *ptr) {

  /* making a pointer to the header of the allocated chunk */
  struct header * head = (void*)((char*)ptr - sizeof(struct header));
  int free_mem_size = head->size + sizeof(struct header);
  /* integrity check */
  if(head->magic != MAGICNUMBER){
    return 1;
  }
  else{
    pthread_mutex_lock(&lock);
    /* making pointers to traverse through the free list to keep free list in
       ascending order in terms of size */
    struct free_mem * current = free_list->next;
    struct free_mem * previous = free_list;
    
    if(current != NULL){
      /* finding appropriate insertion spot for the block to be freed*/
      while((previous->size >= free_mem_size) && 
             ((current == NULL) || (current->size <= free_mem_size))){
        current = current->next;
        previous = previous->next;
      }
    }
    /* making a new free node and adding it to free list at appropriate spot*/
    struct free_mem * new_node = (void *) head;
    new_node->size = free_mem_size - sizeof(struct free_mem);
    new_node->next = current;
    previous->next = new_node;

    //coalesce();

    pthread_mutex_unlock(&lock); 
  }
  return 0;
}

/* coalesce: coalesces adjacent free blocks together*/

//void coalesce(void){

  /* making pointers to traverse through the free list */
  /*

  struct free_mem * current = free_list->next;
  struct free_mem * previous  = free_list;
  //if there is one one node there is nothing to coalesce so function exits
  if (current == NULL){
    exit(0);
  }
  else{ //there is more than one node

    void * prev_addr_end;
    void * curr_addr_start;

    while(current != NULL){
      prev_addr_end = (void*)((char*)previous + (char*)previous->size + (char*)1);
      curr_addr_start = (void *)current;
      if(prev_addr_end == curr_addr_start){
        previous->size = current->size + sizeof(struct free_mem);
        previous->next = current->next;
        if(current->next == NULL){
          current = NULL;
        }
        else{
          current = current->next->next;
        }
      }
      else{
        previous = previous->next;
        current = current->next;
      }
    }
  }
  exit(0);
}
*/