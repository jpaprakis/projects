#include <stdio.h>
#include <stdlib.h>
#include "gc.h"
#include "gc_fstree.h"
#include "fstree.h"

int fscount = 0;

/* mark_tree will traverse the data structure rooted at Fstree
and set mem_chunk to USED if it reaches it in the data
structure
*/
void mark_fstree(void *tree_head) 
{
	Fstree *tree_head_ptr = tree_head;

	//Calls mark_one on the node at tree_head_ptr - if it has already been marked,
	//does not recurse/iterate further
	if (mark_one(tree_head_ptr) != 1)
	{
		fscount++;
		//If this is the first time that node is being marked, marks the name of
		//the node as well
		mark_one(tree_head_ptr->name);
		fscount++;
		//Iterates through all of the node's links and calls mark_one on them. 
		//Recursively calls mark_fstree on each of the links' fstrees. 
		Link *tree_ptr_link = tree_head_ptr->links;
		for (; tree_ptr_link !=NULL; tree_ptr_link = tree_ptr_link->next)
		{
			if (mark_one(tree_ptr_link) !=1)
			{
				fscount++;
			}
			mark_fstree(tree_ptr_link->fptr);
		}
	}
}
