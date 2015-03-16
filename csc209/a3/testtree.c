/* Read and execute a list of operations on an Fstree.
 * Periodically call the garbage collector.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fstree.h"
#include "gc.h"
#include "gc_fstree.h"

extern Fstree *root;
void mark_fstree(void *);


/*count_mem_chunk counts the number of elements in the actual Fstree struct and
compares it with the amount in the mem_chunk struct. If the amount is the same,
and both amounts match the inputted expected_count, the test will return success
*/
void count_mem_chunk(int *count, int expected_count)
{
    struct mem_chunk *mem_head_ptr = mem_chunk_head;
    for (; mem_head_ptr!=NULL; mem_head_ptr=mem_head_ptr->next)
    {
        *count = *count + 1;
    }
    if (*count == expected_count && *count == fscount)
    {
        printf("Success - count is correct!\n");
    }
    else
    {
        printf("Failure - count is %d\n", *count);
    }
    *count = 0;
    fscount =0;
}

int main(int argc, char **argv) {
    //open the logfile in write mode at the very beginning to clear it
    FILE *logfile;
    logfile = fopen(LOGFILE, "w");
    fclose(logfile);

    //adding root to the tree
    root = create_node("root");

    int count = 0;
    fscount = 0;
/*CASE 1: Adding nodes, calling mark and sweep should not delete anything
*/

    //Adding 7 nodes as per instructions in testtreedoc
    do_transactions("testtreedoc");

    //fscount keeps track of the number of elements in the actual Fstree and
    //link data structures - kept track of through gc_fstree's mark_fstree function
    fscount =0;

    //calling mark and sweep on them - should still have the 7 nodes that were 
    //added (including the root). Expecting count to be 23 - allocated 3 chunks 
    //per node and 2 for the root

    mark_and_sweep(root, mark_fstree);

    count_mem_chunk(&count, 23);

/*CASE 2: Adding a hard link to c which was originally in node a, and is now going
to link to node b. Should cause an increase of 1 memory chunk for a link
*/
  
    add_hard_link("b", "a", "c");

    mark_and_sweep(root, mark_fstree);

    count_mem_chunk(&count, 24);

/*CASE 3: Removing 2 nodes from the middle, calling mark and sweep should delete
4 chunks of memory (1 name, 2 links, 1 node) because one of the nodes (c) is 
linked to by b as well, so deleting it from a only deletes its link
*/
    remove_node("a", "d");
    remove_node("a", "c");

    mark_and_sweep(root, mark_fstree);

    count_mem_chunk(&count, 20);

/*CASE 4: Removing 1 node (c) from b, which has another node beneath it that
is now not accessible by any other node - both should be deleted, freeing 6 chunks
*/
    remove_node("b", "c");

    mark_and_sweep(root, mark_fstree);

    count_mem_chunk(&count, 14);

/*CASE 5: Removing the rest of the nodes from the tree, calling mark and sweep
should delete the remaining memory except for 2 chunks belonging to the root
(not three since the root does not have a link allocated for it)
*/
    remove_node("", "a");
    remove_node("", "b");

    mark_and_sweep(root, mark_fstree);

    count_mem_chunk(&count, 2);

/*CASE 6: Marking and sweeing with no changes at all after everything except
the root has been removed
*/
    mark_and_sweep(root, mark_fstree);

    count_mem_chunk(&count, 2);

    return 0;
}
