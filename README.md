# projects
For more information about a project please see the brief description below.

##CSC369
Projects for CSC369: Principles of operating systems (in C)

###a1: Malloc
Manually created malloc and free using sbrk, and saving free chunks of memory in a linkedlist. 

####Instructions to run:
1. make
2. test_malloc -f (filename) -d -t

Where -d (debug, prints out malloced chunks) and -t (touch) are optional 
  
#####Options for (filename):

-traces/1-simple-malloc

-traces/2-extended-malloc

-traces/3-simple-free

-traces/4-reuse

-traces/5-coalesce

-traces/longsingle
    
###a2: Virtual to Physical Memory Simulation
Simulation of translating virtual memory addresses to physical in a computer, and swapping in & out from ROM (swapfile) to RAM (physmem). Takes in a tracefile which contains traces of real programs' virtual memory address accesses & simulates translation using a multi-level page table.

####Instructions to run:
1. make
2. ./sim -f (tracefile) -m (RAM size) -s (ROM size) -a (swapping algorithm)

#####Options for tracefile:
-traceprogs/tr-simpleloop.ref (trace of a program that loops and allocates memory on the stack & heap, then frees it)

-traceprogs/tr-matmul.ref (trace of a standard matrix multiplication program)

-traceprogs/tr-blocked.ref (trace of blocked matrix multiplication program)

-traceprogs/tr-do_fstree.ref (This one is pretty cool, it traces the do_fstree program from the garbage collector below in a loop 50 times)

#####Options for -m:
-make this the same size as -s for no swapping (everything just stored in RAM), or less to force swapping

-for the programs above try: 50, 100, 150, 200

#####Options for -s:
-make this at least as big as the number of unique virtual addresses in the tracefile

-for the programs above, 3000 will do for all

#####Options for -a:
This is the algorithm we use to decide which address to swap out when swapping must occur.

-fifo (this is a standard first in, first out algorithm)

-lru (this is the "least recently used" algorithm, which generally has the best hit rate in a real-world case)

-clock (this is an approximated "least recently used" algorithm, more efficient than lru but lower hit rate

-opt (this is the optimal algorithm, where the item in RAM which will be used furthest into the future is chosen as the victim. This requires knowledge of the future which is only possible in this case because we have tracefiles!)

ie. you can run as follows: ./sim -f tr-matmul.ref -m 50 -s 3000 -a opt

The README.pdf contains an analysis of the different swapping algorithms on different programs.

##CSC209
Projects for CSC209H: Software Tools and Systems Programming (in C)

###a3: Garbage Collector
A mark-and-sweep garbage collector that works on a linkedlist or a fs-tree

####Instructions to run:

1. make

2. choose between two programs:

  
  #####To make a tree:
  
  -do_fstree (this will create a tree based on the specifications in fs_trans2, and garbage collect nodes that were         removed at runtime)

  #####To make a linkedlist:
  
    do_trans (filename)  (this will create a linkedlist based on the specifications in (filename))
    
    -(filename) can either be transactions1 or short_trans
    
    -May send a SIGUSR1 signal to the program to garbage collect at any time while the program is running (generally only transactions1 will run long enough to do this)

###a4: Calserver
Set up the server side of an online calendar tool that allows multiple clients to connect and create users and events, and add other users to events. 

Difficult to run as the client is not set up.
