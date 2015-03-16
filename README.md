# projects
For more information about a project please see the brief description below.

##CSC369
Projects for CSC369: Principles of operating systems (in C)

###a1: Malloc
Manually created malloc and free using sbrk, and saving free chunks of memory in a linkedlist. 

Instructions to run:
1. make
2. test_malloc -f (filename) -d -t

Where -d (debug, prints out malloced chunks) and -t (touch) are optional 
  
Options for (filename):
traces/1-simple-malloc
traces/2-extended-malloc
traces/3-simple-free
traces/4-reuse
traces/5-coalesce
traces/longsingle
    
###a2: 

##CSC209
Projects for CSC209H: Software Tools and Systems Programming (in C)

###a3: Garbage Collector
A mark-and-sweep garbage collector that works on

Instructions to run:
1. make
2. choose between two programs:
  
  To make a tree:
  -do_fstree (this will create a tree based on the specifications in fs_trans2, and garbage collect nodes that were         removed at runtime)

  To make a linkedlist:
    do_trans <filename>  (this will create a linkedlist based on the specifications in <filename>)
    -<filename> can either be transactions1 or short_trans
    -May send a SIGUSR1 signal to the program to garbage collect at any time while the program is running (generally only      transactions1 will run long enough to do this)

###a4: Calserver
Set up the server side of an online calendar tool that allows multiple clients to connect and create users and events, and add other users to events. 
Difficult to run as the client is not set up.
