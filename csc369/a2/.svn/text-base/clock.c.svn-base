#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

//Global i to keep track of where the "clock hand" is currently at in the coremap
unsigned int i = 0;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	
	//use an infinite loop to scan through everything in the coremap until you find 
	//one with ref = 0 if i gets too high, set it back to 0 and keep scanning
	
	unsigned int victim_frame;
	while(1)
	{
		//Looping i back around
		if (i > memsize-1)
		{
			i = 0;
		}
		
		//if this index of coremap has its ref bit set, set it back to 0
		if ( ((coremap[i].pte)->frame) & PG_REF )
		{
			(coremap[i].pte)->frame &= ~PG_REF;
		}

		//Otherwise, we found our victim!!
		else
		{
			break;
		}

		i ++;
	}

	victim_frame = i;

	//Set i to start at the next coremap frame next time
	if (i < memsize-1)
	{
		i++;
	}
	
	//if victim was memsize-1, set i to start from beginning
	else 
	{
		i = 0;
	}

	return victim_frame;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {

	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {

}
