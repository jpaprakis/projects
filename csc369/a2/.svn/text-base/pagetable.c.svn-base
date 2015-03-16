#include <assert.h>
#include <string.h> 
#include "sim.h"
#include "pagetable.h"


// The top-level page table (also known as the 'page directory')
pgdir_entry_t pgdir[PTRS_PER_PGDIR]; 

// Counters for various events.
// My code increments when these related events occur
int hit_count = 0;
int miss_count = 0;
int ref_count = 0;
int evict_clean_count = 0;
int evict_dirty_count = 0;
/*
 * Allocates a frame to be used for the virtual page represented by p.
 * If all frames are in use, calls the replacement algorithm's evict_fcn to
 * select a victim frame.  Writes victim to swap if needed, and updates 
 * pagetable entry for victim to indicate that virtual page is no longer in
 * (simulated) physical memory.
 *
 * Counters for evictions are updated appropriately in this function.
 */
int allocate_frame(pgtbl_entry_t *p) {
	int i;
	int frame = -1;
	for(i = 0; i < memsize; i++) {
		if(!coremap[i].in_use) {
			frame = i;
			break;
		}
	}
	if(frame == -1) { // Didn't find a free page.
		
		// Call replacement algorithm's evict function to select victim
		frame = evict_fcn();

		// All frames were in use, so victim frame must hold some page
		// Write victim page to swap, if needed, and update pagetable

		//Ensuring the victim is valid and not onswap
		if (!((coremap[frame].pte)->frame) & PG_VALID)
		{
			perror("Error: frame we are swapping out from coremap must be valid");
			exit(1);
		}

		if (((coremap[frame].pte)->frame) & PG_ONSWAP)
		{
			perror("Error: frame we are swapping out from coremap must not "
				"already be on swap!");
			exit(1);
		}

		//Toggle valid & onswap to set the valid bit of the victim's virtual page 
		//table entry to 0 and the onswap bit to 1 
		(coremap[frame].pte)->frame &= ~PG_VALID;
		(coremap[frame].pte)->frame |= PG_ONSWAP;

		//Check the dirty bit - if high, we need to swap the page out to the 
		//swapfile. Otherwise it's already up to date in the swapfile, can just 
		//overwrite it in physical memory!
		if (((coremap[frame].pte)->frame) & PG_DIRTY)
		{
			evict_dirty_count ++;

			//Swap the victim out, and store its offset in its virtual pte
			int new_offset;
			new_offset = swap_pageout((coremap[frame].pte)->frame >> PAGE_SHIFT, 
				(coremap[frame].pte)->swap_off);
			(coremap[frame].pte)->swap_off = new_offset;
		}
		else 
		{
			evict_clean_count ++;
		}
	}

	// Record information for virtual page that will now be stored in frame
	coremap[frame].in_use = 1;
	coremap[frame].pte = p;

	return frame;
}

/*
 * Initializes the top-level pagetable.
 * This function is called once at the start of the simulation.
 * For the simulation, there is a single "process" whose reference trace is 
 * being simulated, so there is just one top-level page table (page directory).
 * To keep things simple, we use a global array of 'page directory entries'.
 *
 * In a real OS, each process would have its own page directory, which would
 * need to be allocated and initialized as part of process creation.
 */

void init_pagetable() {
	int i;
	// Set all entries in top-level pagetable to 0, which ensures valid
	// bits are all 0 initially.
	for (i=0; i < PTRS_PER_PGDIR; i++) {
		pgdir[i].pde = 0;
	}
}

// For simulation, we get second-level pagetables from ordinary memory
pgdir_entry_t init_second_level() {
	int i;
	pgdir_entry_t new_entry;
	pgtbl_entry_t *pgtbl;

	// Allocating aligned memory ensures the low bits in the pointer must
	// be zero, so we can use them to store our status bits, like PG_VALID
	if (posix_memalign((void **)&pgtbl, PAGE_SIZE, 
			   PTRS_PER_PGTBL*sizeof(pgtbl_entry_t)) != 0) {
		perror("Failed to allocate aligned memory for page table");
		exit(1);
	}

	// Initialize all entries in second-level pagetable
	for (i=0; i < PTRS_PER_PGTBL; i++) {
		pgtbl[i].frame = 0; // sets all bits, including valid, to zero
		pgtbl[i].swap_off = INVALID_SWAP;
	}

	// Mark the new page directory entry as valid
	new_entry.pde = (uintptr_t)pgtbl | PG_VALID;

	return new_entry;
}

/* 
 * Initializes the content of a (simulated) physical memory frame when it 
 * is first allocated for some virtual address.  Just like in a real OS,
 * we fill the frame with zero's to prevent leaking information across
 * pages. 
 * 
 * In our simulation, we also store the the virtual address itself in the 
 * page frame to help with error checking.
 *
 */
void init_frame(int frame, addr_t vaddr) {


	// Calculate pointer to start of frame in (simulated) physical memory
	char *mem_ptr = &physmem[frame*SIMPAGESIZE];
	// Calculate pointer to location in page where we keep the vaddr

    addr_t *vaddr_ptr = (addr_t *)(mem_ptr + sizeof(int));
	
	memset(mem_ptr, 0, SIMPAGESIZE); // zero-fill the frame
	*vaddr_ptr = vaddr;             // record the vaddr for error checking
	return;
}

/*
 * Locate the physical frame number for the given vaddr using the page table.
 *
 * If the entry is invalid and not on swap, then this is the first reference 
 * to the page and a (simulated) physical frame should be allocated and 
 * initialized (using init_frame).  
 *
 * If the entry is invalid and on swap, then a (simulated) physical frame
 * should be allocated and filled by reading the page data from swap.
 *
 * Counters for hit, miss and reference events should be incremented in
 * this function.
 */

char *find_physpage(addr_t vaddr, char type) {

	pgtbl_entry_t *p=NULL; // pointer to the full page table entry for vaddr
	unsigned idx = PGDIR_INDEX(vaddr); // get index into page directory

	// Use top-level page directory to get pointer to 2nd-level page table

	//If first level pgdir entry does not have a valid second level initalized, 
	//initialize its second level now
	if (!(pgdir[idx].pde & PG_VALID)) 
	{
		pgdir[idx] = init_second_level();
	}

	pgtbl_entry_t *second_level_table = (pgtbl_entry_t*) pgdir[idx].pde;

	// Use vaddr to get index into 2nd-level page table and initialize 'p'
	unsigned second_idx = PGTBL_INDEX(vaddr); //get index into page table
	
	//Setting p
	p = &(second_level_table[second_idx]);

	//If both valid and onswap bits are low, we are accessing this
	//memory for the first time and must initalize it
	if ((!(p->frame & PG_VALID)) && (!(p->frame & PG_ONSWAP)))
	{
		unsigned frame_to_allocate = allocate_frame(p);
		init_frame(frame_to_allocate, vaddr);

		//Set the bits of the corresponding physical frame
		p->frame = frame_to_allocate << PAGE_SHIFT;

		//Set the valid bit
		p->frame |= PG_VALID;
		
		//Set the dirty bit regardless of instruction type, since
		//this is not on the swapfile at all
		p->frame |= PG_DIRTY;

		miss_count ++;
	}
	
	//If valid bit is low but onswap bit is high, we must swap this
	//memory back in and allocate it a physical page frame
	else if((!(p->frame & PG_VALID)) && (p->frame & PG_ONSWAP))
	{
		//Find a frame in physical memory to swap into
		unsigned frame_to_allocate = allocate_frame(p);

		//Swap this address back into physical memory
		if (swap_pagein(frame_to_allocate, p->swap_off))
		{
			perror("Error swapping in!");
			exit(1);
		}

		//Set the bits of the corresponding physical frame
		p->frame = frame_to_allocate << PAGE_SHIFT;

		//Set the valid bit, and ensure onswap bit is off
		p->frame |= PG_VALID;
		p->frame &= ~PG_ONSWAP;

		//Dirty bit will be set to 1 if the instruction was S or M
		if ((type == 'S') || (type == 'M'))
		{
			p->frame |= PG_DIRTY;
		}

		miss_count ++;
	}
	//Otherwise, this address is already in physical memory: no
	//need to swap it or anything, we got a hit!
	else 
	{
		//If instruction was S or M, set the dirty bit
		if ((type == 'S') || (type == 'M'))
		{
			p->frame |= PG_DIRTY;
		}
		hit_count ++;
	}

	//p was referenced, so mark it as such
	p->frame |= PG_REF; 


	// Call replacement algorithm's ref_fcn for this page
	ref_fcn(p);
	ref_count ++;

	// Return pointer into (simulated) physical memory at start of frame
	return  &physmem[(p->frame >> PAGE_SHIFT)*SIMPAGESIZE];
}

void print_pagetbl(pgtbl_entry_t *pgtbl) {
	int i;
	int first_invalid, last_invalid;
	first_invalid = last_invalid = -1;

	for (i=0; i < PTRS_PER_PGTBL; i++) {
		if (!(pgtbl[i].frame & PG_VALID) && 
		    !(pgtbl[i].frame & PG_ONSWAP)) {
			if (first_invalid == -1) {
				first_invalid = i;
			}
			last_invalid = i;
		} else {
			if (first_invalid != -1) {
				printf("\t[%d] - [%d]: INVALID\n",
				       first_invalid, last_invalid);
				first_invalid = last_invalid = -1;
			}
			printf("\t[%d]: ",i);
			if (pgtbl[i].frame & PG_VALID) {
				printf("VALID, ");
				if (pgtbl[i].frame & PG_DIRTY) {
					printf("DIRTY, ");
				}
				printf("in frame %d\n",pgtbl[i].frame >> PAGE_SHIFT);
			} else {
				assert(pgtbl[i].frame & PG_ONSWAP);
				printf("ONSWAP, at offset %lu\n",pgtbl[i].swap_off);
			}			
		}
	}
	if (first_invalid != -1) {
		printf("\t[%d] - [%d]: INVALID\n", first_invalid, last_invalid);
		first_invalid = last_invalid = -1;
	}
}

void print_pagedirectory() {
	int i; // index into pgdir
	int first_invalid,last_invalid;
	first_invalid = last_invalid = -1;

	pgtbl_entry_t *pgtbl;

	for (i=0; i < PTRS_PER_PGDIR; i++) {
		if (!(pgdir[i].pde & PG_VALID)) {
			if (first_invalid == -1) {
				first_invalid = i;
			}
			last_invalid = i;
		} else {
			if (first_invalid != -1) {
				printf("[%d]: INVALID\n  to\n[%d]: INVALID\n", 
				       first_invalid, last_invalid);
				first_invalid = last_invalid = -1;
			}
			pgtbl = (pgtbl_entry_t *)(pgdir[i].pde & PAGE_MASK);
			printf("[%d]: %p\n",i, pgtbl);
			print_pagetbl(pgtbl);
		}
	}
}

const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}