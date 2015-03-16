#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gc.h"
#include "gc_list.h"
#include <unistd.h>

//Declaring a global pointer to the head of the mem_chunk list list
//static struct mem_chunk* mem_chunk_head;
struct mem_chunk* mem_chunk_head;

//Declaring debug to print for debugging purposes
static int debug = 0;

//Declaring count of allocated memory each time gc_malloc is called
static int a_count = 0;

//Declaring count of deleted memory each time something is freed by
//the garbage collector
static int d_count = 0;

//Declaring count of remaining memory allocations once memory is freed
//by the garbage collector
static int r_count = 0;

//Declaring count of amount that was remaining the last time, plus the
//memory that was added
static int lr_count = 0;


/* A wrapper around malloc to keep track of dynamically allocated memory
 */
void *gc_malloc(int nbytes)
{

	if (debug)
	{
		printf("mallocing\n");
		sleep(3);
	}

	/*Step 1: Mallocing & Adding the mem_chunk structure which will keep track
		of everything malloced in other data structure
	*/
	
	struct mem_chunk* new_mem_chunk;

	//mallocing for the mem_chunk structure
	if ((new_mem_chunk = malloc(sizeof(struct mem_chunk))) == NULL)
	{
		perror("malloc");
		exit(1);
	}
	a_count++;
	
	//Case where there are no elements in the mem_chunk structure
	if (mem_chunk_head == NULL)
	{
		new_mem_chunk->next = NULL;
	}

	//Case where there are already elements in the mem_chunk structure
	else
	{
		new_mem_chunk->next = mem_chunk_head;
	}

	//Adding to the head of the mem_chunk structure each time
	mem_chunk_head = new_mem_chunk;

	/*Step 2: Mallocing & Adding the actual structure which will be kept track
		of by the mem_chunk structure above
	*/

	//mallocing for the actual data structure, and setting the mem_chunk 
	//structure's address to point to it
	if ((new_mem_chunk->address = malloc(nbytes))==NULL)
	{
		perror("malloc");
		exit(1);
	}
	a_count++;
	
	//Setting the memory to USED to begin with
	new_mem_chunk->in_use = USED;
	
	if (debug)
	{
		printf("just allocated at address %s\n", (char*)&(new_mem_chunk->address));
		printf("ending malloc\n");
	}

	return new_mem_chunk->address;
}


/* Executes the garbage collector.
 * mark_obj will traverse the data structure rooted at obj, calling
 * mark_one for each dynamically allocated chunk of memory to mark
 * that it is still in use.
 */
void mark_and_sweep(void *obj, void (*mark_obj)(void *))
{

	//Opening the logfile for appending, will log more information each time
	//mark_and_sweep is called
	FILE * fout;
	fout = fopen(LOGFILE, "a");

	struct mem_chunk *tempptr;

	if (debug)
	{
		printf("marking and sweeping!\n");
		tempptr = mem_chunk_head;

		for(; tempptr; tempptr = tempptr->next)
		{
			printf("before deletion: %s\n", (char*)&tempptr);
		}
	}

	//first resetting the mem_chunk list by iterating through it and setting all 
	//in_use values to NOT_USED
	tempptr = mem_chunk_head;
	for (; tempptr; tempptr = tempptr->next)
	{
		if (debug)
			{
				printf("reset - tempptr is %s\n", (char*)&tempptr);
			}

		tempptr->in_use = NOT_USED;
	}

	//mark_obj will modify the mem_chunk list - will
	//mark those which are still accessible 
	(*mark_obj)(obj);

	//traverse mem_chunk linked list object and check for unmarked objects,
	//freeing whatever has not been marked
	tempptr = mem_chunk_head;

	//prev_tempptr points to the object before tempptr
	struct mem_chunk *prev_tempptr = NULL;
	
	//next_tempptr points to the object after tempptr
	struct mem_chunk *next_tempptr;

	//This while loop will traverse the mem_chunk list and free anything not in_use
	//until it gets to the very last item in the list, then it will exit. It
	//will not check or free the very last item in the list.
	while (tempptr!=NULL && tempptr->next !=NULL) 
	{

		next_tempptr = tempptr->next;	

		//If NOT_USED, will free both the memory that the mem_chunk's address is
		//pointing to, as well as the actual mem_chunk item
		if (tempptr->in_use == NOT_USED)
		{
			free(tempptr->address);
			d_count++;
			free(tempptr);
			d_count++;

			//If the head of the mem_chunk list was just freed, the head will now
			//be set as next_tempptr
			if (prev_tempptr == NULL)
			{
				mem_chunk_head = next_tempptr;
			}

			//If there is an element stored in the mem_chunk list head that was
			//not freed, the previous element's next will now point to the next
			//element, since the one in between them was freed
			else
			{
				prev_tempptr->next = next_tempptr;
			}
		}

		//If the current tempptr is still in use and not freed, the previous
		//tempptr will now point to the current tempptr, since there is an
		//element there to point to and it will remain in the mem_chunk list
		else
		{
			prev_tempptr = tempptr;
		}

		tempptr = next_tempptr;
	}

	//At this point, tempptr will point to the very last node in the mem_struct
	if (tempptr!=NULL && tempptr->in_use == NOT_USED)
		{
			free(tempptr->address);
			d_count++;
			free(tempptr);
			d_count++;

			//If deleting the last node in the mem_struct and there's other nodes
			//in it that still exist, set the previous node's next to NULL
			if (prev_tempptr != NULL)
				{
				prev_tempptr->next = NULL;
				}

			//Case where nothing exists in the mem_struct at all
			else
			{
				mem_chunk_head = NULL;
			}
		}
	
	//This loop exists to count the number of items remaining in the mem_chunk
	//list for logging purposes
	tempptr = mem_chunk_head;
	for (; tempptr!=NULL; tempptr=tempptr->next)
	{
		r_count++;
		if (debug)
		{
			printf("after deletion %s\n", (char*)&tempptr);
		}
	}

	//Printing to the log after mark_and_sweep has completed
	fprintf(fout, "added: %d, deleted: %d, ", a_count, d_count);
	fprintf(fout, "remaining from this round: %d, ", r_count*2);
	fprintf(fout, "remaining from last round: %d\n",  lr_count);

	//Resetting values of counts after reporting to the log
	lr_count = r_count*2;
	a_count = 0;
	d_count = 0;
	r_count = 0;
	fclose(fout);
}


/* Mark vptr as still in use
 * Return code:
 *   0 on success
 *   1 if memory chunk pointed to by vptr was already marked as in use
 */
int mark_one(void *vptr)
{
	if(debug)
	{
		printf("marking one\n");
	}

	struct mem_chunk *tempptr;
	tempptr = mem_chunk_head;
	
	//Iterates through everything in the mem_chunk list until it matches the
	//item at mem_chunk->address with vptr 
	for (; tempptr!=NULL; tempptr = tempptr->next)
	{
		if ((tempptr->address) == vptr)
		{
			if ((tempptr->in_use) == NOT_USED)
			{
				tempptr->in_use =USED;
				return 0;
			}
			else
			{
				return 1;
			}
		}
	}
	//mark_one should always find the item at vptr under some mem_chunk->address
	//otherwise there is a problem - should never get down to this part of code
	perror("data structure not in mem_chunk list");
	exit(1);
}
