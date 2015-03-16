/* Read and execute a list of operations on a linked list.
 * Periodically call the garbage collector.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "gc.h"
#include "gc_list.h"

static List *ll = NULL;

void mark_list(void *);

//test_structs compares the List struct with the mem_chunk struct's addresses
//and ensures that they are identical - if so, will return 1. Else will return 0.
//Count is increased every time it runs through the list
int test_structs(int *count) {
    struct mem_chunk *mem_head_ptr = mem_chunk_head;
    List *list_head_ptr = ll;
    int found = 1;
    while(mem_head_ptr!=NULL)
    {
        *count = *count+1;
        if (list_head_ptr != mem_head_ptr->address)
        {
            found = 0;
        }
        mem_head_ptr = mem_head_ptr->next;
        list_head_ptr = list_head_ptr->next;
    }
    return found;
}

void tests(int *count, int req_count) {
    //Integrity test
    if (test_structs(count)==1)
    {
        printf ("Success - Matching test passed!\n");

        //If integrity test passes, go to count test
        if (*count == req_count)
        {
            printf("Success - Count test passed!\n");
        }
        else
        {
            printf("Failure - Count test failed, count is %d\n", *count);
        }
    }
    else
    {
        printf("Failure - Matching test failed\n");
    }
    *count = 0;
}

int main(int argc, char **argv) {
    //open the logfile in write mode at the very beginning to clear it
    //mem_chunk_head = NULL;
    FILE *logfile;
    logfile = fopen(LOGFILE, "w");
    fclose(logfile);

    int count = 0;


/*CASE 1: ADDING NODES
*/

    //adding 5 nodes
    ll = add_node(ll, 1);
    ll = add_node(ll, 2);
    ll = add_node(ll, 3);
    ll = add_node(ll, 4);
    ll = add_node(ll, 5);

    //calling mark and sweep on them - should still have the
    //5 nodes that were added
    mark_and_sweep(ll, mark_list);

    //calling tests with a required count of 5 to pass the test
    tests(&count, 5);


/*CASE 2: DELETING 2 NODES FROM THE MIDDLE
*/

    //deleting the middle and 2nd node
    ll = remove_node(ll, 3);
    ll = remove_node(ll, 2);

    //calling mark and sweep - should have all nodes
    //except the middle one and 2nd one
    mark_and_sweep(ll, mark_list);

    //calling tests with a required count of 3 to pass the test
    tests(&count, 3);


/*CASE 3: MARKING AND SWEEPING AFTER NO CHANGES
*/

    //calling mark_and_sweep again before any other changes have happened -
    //should be the exact same as previous, with a count of 3 to pass the test
    mark_and_sweep(ll, mark_list);

    //calling tests with a required count of 3 to pass the test
    tests(&count, 3);


/*CASE 4: REMOVING THE TAIL NODE
*/

    //deleting the tail node
    ll = remove_node(ll, 1);

    //calling mark and sweep - should only have nodes 4 and 5 left
    mark_and_sweep(ll, mark_list);

    //calling tests with a required count of 2 to pass the test
    tests(&count, 2);


/*CASE 5: REMOVING THE HEAD NODE
*/

    //deleting the head node, 5
    ll = remove_node(ll, 5);

    //calling mark and sweep again - should be 1 node left
    mark_and_sweep(ll, mark_list);

    //calling tests with a required count of 1 to pass the test
    tests(&count, 1);


/*CASE 6: DELETING THE LAST REMAINING NODE
*/

    //deleting the final node, 4
    ll = remove_node(ll, 4);

    //calling mark and sweep - should have no more nodes
    mark_and_sweep(ll, mark_list);

    //calling tests with a required count of 0 to pass the test
    tests(&count, 0);

/*CASE 7: MARKING & SWEEPING WHEN EMPTY AND NO CHANGES MADE
*/

    //calling mark and sweep again - should still have no more nodes
    mark_and_sweep(ll, mark_list);

    //calling tests one last time with a required count of 0 to pass the test
    //tests(&count, 0);


    return 0;
}
