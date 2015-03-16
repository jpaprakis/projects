#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include "sim.h"

extern int debug;

extern struct frame *coremap;

extern char *tracefile;

//We will be using a pgdir_entry_t struct as a top level struct
pgdir_entry_t opt_pgdir[PTRS_PER_PGDIR];

//Global variables to keep track of where we are in the trace file
int init_line_count = 1;
int runtime_line_count = 1;

void opt_store();

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {

	int largest_diff = 0;
	int victim;
	int coremap_frame;
	opt_second_lvl *my_node = NULL;

	//Loop through all the things in the coremap frame and check which one
	//has the greatest difference between where it appears in the file, vs
	//where we currently are in the file
	for (coremap_frame = 0; coremap_frame < memsize; coremap_frame ++)
	{
		//Getting the virtual address from physical memory
		char *memptr = &physmem[coremap_frame*SIMPAGESIZE];
		addr_t *checkaddr = (addr_t *)(memptr + sizeof(int));

		//Now index into the address in the multi level page table that
		//was set up below!
		unsigned idx = PGDIR_INDEX(*checkaddr); // get index into page directory
		opt_second_lvl* pde_ptr = (opt_second_lvl*) opt_pgdir[idx].pde;
		unsigned second_idx = PGTBL_INDEX(*checkaddr); //get index into page table

		my_node = &(pde_ptr[second_idx]);

		//Now loop through all the occurences of this virtual address
		//As we go, "clean up" parts of the list we don't care about
		//anymore so we don't have to loop through them again later
		struct lru_list *occ_ptr = my_node->occurences;

		for (; occ_ptr; occ_ptr = occ_ptr->next)
		{
			int opt_diff = (occ_ptr->frame)-runtime_line_count;

			//If the difference is less than 0, it's already passed:
			//we don't really care about it!
			if (opt_diff < 0)
			{
				my_node->occurences = occ_ptr->next;
			}

			//If we found our first instance where it's greater than 0,
			//use this to calculate the difference and see if it's the victim!
			else if ( opt_diff > 0)
			{
				if (opt_diff > largest_diff)
				{
					victim = coremap_frame;
					largest_diff = opt_diff;
				}
				break;	
			}
		}

		//At the end, if opt_diff is never greater than 0, we know that this
		//address will never occur again: this is the victim!
		if ( occ_ptr == NULL)
		{
			victim = coremap_frame;
			break;
		}

	}
	
	return victim;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	//Increase the global line count after every reference
	runtime_line_count++;
	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {

	//read in the accesses from file

	FILE *trace_fp;
	if((trace_fp = fopen(tracefile, "r")) == NULL) {
		perror("Error opening tracefile:");
		exit(1);
	}
	char opt_buf[MAXLINE];
	addr_t opt_vaddr = 0;
	char opt_type;

	while(fgets(opt_buf, MAXLINE, trace_fp) != NULL) {
		if(opt_buf[0] != '=') {
			sscanf(opt_buf, "%c %lx", &opt_type, &opt_vaddr);

			//Call opt_store which indexes to the vaddr and stores its current
			//line number in a linkedlist
			opt_store(opt_vaddr);
			init_line_count ++;
		} else {
			continue;
		}
	}
}

void opt_store(addr_t opt_vaddr)
{
	opt_second_lvl *my_node = NULL;

	unsigned idx = PGDIR_INDEX(opt_vaddr); // get index into page directory
	
	//If we're bringing it in for the first time, initialize second level
	if (!(opt_pgdir[idx].pde & PG_VALID))
	{
		// Allocating aligned memory ensures the low bits in the pointer must
		// be zero, so we can use them to store our status bits, like PG_VALID
	
		opt_second_lvl *pgtbl;

		if (posix_memalign((void **)&pgtbl, PAGE_SIZE, 
			PTRS_PER_PGTBL*sizeof(opt_second_lvl)) != 0) {
			perror("Failed to allocate aligned memory for page table");
			exit(1);
		}

		opt_pgdir[idx].pde = (uintptr_t)pgtbl | PG_VALID;		
	}

	unsigned second_idx = PGTBL_INDEX(opt_vaddr); //get index into page table
	
	opt_second_lvl* pde_ptr = (opt_second_lvl*) opt_pgdir[idx].pde;

	my_node = &(pde_ptr[second_idx]);

	//The case where this is the head
	if(!my_node->is_init)
	{
		my_node->occurences = malloc(sizeof (struct lru_list));
		my_node->occurences->frame = init_line_count;
		my_node->occurences->next = NULL;
		my_node->is_init = 1;

		//Set the end pointer, same as the head in this case
		my_node->end_ptr = my_node->occurences;
	}

	//The case where this is not the head
	else
	{
		struct lru_list *new_node = malloc(sizeof (struct lru_list));
		new_node->frame = init_line_count;
		new_node->next = NULL;

		//Add the new node after the current end pointer, then set the end 
		//pointer to point to it
		my_node->end_ptr->next = new_node;
		my_node->end_ptr = my_node->end_ptr->next;
	}
}
