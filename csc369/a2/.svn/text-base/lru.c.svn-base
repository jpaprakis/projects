#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

struct lru_list *lru_head = NULL;
struct lru_list *end_ptr = NULL;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	//The last used thing will be at the top of the list, just return its frame
	//and we're done!

	return lru_head->frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {

	//Find the node corresponding to the phys frame of p
	int frame_to_update = p->frame >> PAGE_SHIFT;
	struct lru_list *list_ptr = lru_head;
	for (; list_ptr; list_ptr = list_ptr->next)
	{
		if (list_ptr->frame == frame_to_update)
		{
			break;
		}
	}

	//Now we have the node we're looking for, remove it

	//Case where node was at the head
	if (list_ptr == lru_head)
	{
		lru_head = lru_head->next;
	}

	//Case where it was not at the head
	else 
	{
		//Take care of the case where node not at the head: keep track of what's 
		//before it
		struct lru_list *before_ptr = lru_head;
		struct lru_list *cur_ptr = lru_head->next;

		for (; cur_ptr; cur_ptr=cur_ptr->next)
		{
			if(cur_ptr == list_ptr)
			{
				before_ptr->next = cur_ptr->next;
				break;
			}
			before_ptr = before_ptr->next;
		}
	}

	//Now place our node after the end pointer, and set the end pointer
	//to the new node
	end_ptr->next = list_ptr;
	end_ptr = list_ptr;
	list_ptr->next = NULL;
	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {

	//initialize all nodes in the linkedlist
	if (!(lru_head = malloc(sizeof(struct lru_list)))) {
		perror("malloc");
		exit(1);
	} 
	lru_head->frame = 0;

	struct lru_list *list_ptr = lru_head;
	
	int i;
	for (i = 1; i < memsize; i ++)
	{
		list_ptr->next = malloc(sizeof(struct lru_list));
		list_ptr->next->frame = i;
		list_ptr = list_ptr->next;
	}
	list_ptr->next = NULL;

	end_ptr = list_ptr;

	list_ptr = lru_head;
}