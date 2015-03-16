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
#include <signal.h>

#define MAX_LINE 128
#define ADD_NODE 1
#define DEL_NODE 2
#define PRINT_LIST 3


//Declaring ll as a static variable so that it can be called in the sig handler
static List *ll = NULL;

void mark_list(void *);

//Declaring the sig_handler so that mark_and_sweep is called each time SIGUSR1
//is recieved by do_fstree
void sig_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        mark_and_sweep(ll, mark_list);
    }
}


int main(int argc, char **argv) {
    
    //Clearing the logfile at the beginning of the program
    FILE *logfile;
    logfile = fopen(LOGFILE, "w");
    fclose(logfile);
    
    char line[MAX_LINE];
    char *str;

    if(argc != 2) {
        fprintf(stderr, "Usage: do_trans filename\n");
        exit(1);
    }

    FILE *fp;
    if((fp = fopen(argv[1], "r")) == NULL) {
        perror("fopen");
        exit(1);
    }

    int count = 1;

    //Installing the signal handler in the program
    signal(SIGUSR1, sig_handler);
    
    while(fgets(line, MAX_LINE, fp) != NULL) {

        char *next;
        int value;
        int type = strtol(line, &next, 0);

        switch(type) {
            case ADD_NODE :

                //First blocking any signals so that they don't interrupt when
                //ADD_NODE case is being executed

                ;/* Initialize signal set */
                sigset_t sigs;
                sigemptyset(&sigs);
                /* Add SIGINT to the set */
                sigaddset(&sigs, SIGUSR1);    
                /* Add signals to blocked set (save old mask) */
                sigset_t old_sigs;
                if(sigprocmask(SIG_BLOCK, &sigs, &old_sigs) != 0) {
                    exit(1);
                }

                //Executing rest of ADD_NODE case
                value = strtol(next, NULL, 0);
                ll = add_node(ll, value);

                //unblocking sigs
                if(sigprocmask(SIG_SETMASK, &old_sigs, NULL) != 0) {
                    exit(1);
                }

                break;
            case DEL_NODE :
                value = strtol(next, NULL, 0);
                ll = remove_node(ll, value);
                break;
            case PRINT_LIST :
                str = tostring(ll);
                printf("List is %s\n", str);
                break;
            default :
                fprintf(stderr, "Error: bad transaction type\n");

        }
    
        
        if(count % 10 == 0) {
            mark_and_sweep(ll, mark_list);
            
            // char check[MAX_LINE];
            // fgets(check, MAX_LINE, stdin);
        }       
        count++;

    }
    return 0;
}
