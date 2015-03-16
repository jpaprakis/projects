#include <stdio.h>
#include "fstree.h"
#include "gc.h"
#include "gc_fstree.h"
#include <signal.h>

extern Fstree *root;

//Declaring the sig_handler so that mark_and_sweep is called each time SIGUSR1
//is recieved by do_fstree
void sig_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        mark_and_sweep(root, mark_fstree);
    }
}

int main() {

    //Clearing the logfile at the beginning of the program
    FILE *logfile;
    logfile = fopen(LOGFILE, "w");
    fclose(logfile);

    //Installing the signal handler in the program
    signal(SIGUSR1, sig_handler);

int i;
for (i=0; i<50; i++)
{

    root = create_node("root");


    /*
    do_transactions("fs_trans1");
     print_tree(root, 0);

    mark_and_sweep(root, mark_tree);
    //mark_and_sweep(root, mark_tree);

    print_tree(root, 0);
    */

    do_transactions("fs_trans2");
    mark_and_sweep(root, mark_fstree);
    print_tree(root, 0);
   }
    //print_tree(root, 0);
    //do_transactions("fs_trans_loop");
    // don't call print_tree once you have a loop. It isn't sophisticated 
    // enough to handle the loop

/*
    Or you can put the calls directly in here but be careful, you
    can't use string literals for the paths or you will get Segmentation errors.

    add_node("","one");  // add_node (path, name)
    print_tree(root, 0);
    add_node("one","oneone");  // add_node (path, name)
    print_tree(root, 0);
    printf("************\n");

    char dest_path[20] = "one/oneone";
    char src_path[20] = "";
    char name[10] = "one";

    // this makes a link as a child of  one/oneone back to /one
    add_hard_link(dest_path, src_path, name);
    // DON"T call print_tree now since it doesn't handle loops and 
    //  will print infinitely
    // print_tree(root, 0);

*/
    return 0;
}
