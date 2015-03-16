#include <stdio.h>
#include <stdlib.h>
#include "gc.h"
#include "gc_list.h"
#include "list.h"
#include <unistd.h>

/*mark_list will traverse the data structure rooted at obj
and set mem_chunk to USED if it reaches it in the data
structure
*/
void mark_list(void *list_head) 
{
	List *list_head_ptr = list_head;

	//Iterates through the list structure and calls mark_one on each element.
	//mark_one should never return 1 since the list is linear, and the entire
	//mem_chunk list should have reset before this function is called
	for (; list_head_ptr!=NULL; list_head_ptr = list_head_ptr->next)
	{
		if (mark_one(list_head_ptr) == 1)
		{
			perror("mark_one reset");
			exit(1);
		}
	}
}
