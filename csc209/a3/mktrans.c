/*
 * Generate a random series of add, delete, and print operations on a 
 * linked list.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "list.h"
 
#define MAX_VAL 10000

/* The probability of generating print and add operations
 * The probability of generating a delete operation is 
 * 1 - (PROB_PRINT + PROB_ADD)
 */
#define PROB_PRINT 0.05
#define PROB_ADD 0.55

void printing(int argc, int (*printf)(const char *toprint));

int main(int argc, char **argv) {
    long type;
    long value;
    int length = 0;
    List *ll;

    //If more than 1 arguments are put in, changes the filestream to 
    //be the specified filename at argv[1]
    FILE * out;
    if (argc > 1)
    {
        out = fopen(argv[1], "w");
    }

    //If only 1 argument is put in, the filestream is
    //stdout
    else
    {
        out = stdout;
    }

    int i;
    for(i = 0; i < MAX_VAL; i++) {
        double prob = (double)random() / RAND_MAX;
        if(prob < PROB_PRINT) {
            type = 3;
            fprintf(out, "%ld\n", type);
        }
        if(prob < PROB_PRINT + PROB_ADD) {
            type = 1;
            value = random() % MAX_VAL;
            ll = add_node(ll, value);
            length++;
            fprintf(out, "%ld %ld\n", type, value);
        } else {
            type = 2;
            if(length > 0) {
                // choose a node that is in the list to delete
                int index = random() % length;
                value = find_nth(ll, index);
                ll = remove_node(ll, value);
                length--;
                fprintf(out, "%ld %ld\n", type, value);
            } 
        }
    }
    return 0;
}
