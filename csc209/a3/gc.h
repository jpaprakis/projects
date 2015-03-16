/* External functions that are the interface to the garbage collector */

#ifndef GC_HEADER
#define GC_HEADER

#ifndef LOGFILE
#define LOGFILE "gc.log"
#endif


#define NOT_USED 0
#define USED 1

struct mem_chunk {
    int in_use;
    void * address;
    struct mem_chunk *next;
};

//Setting mem_chunk_head to external so that its contents can be accessed by
//the test programs as well
extern struct mem_chunk *mem_chunk_head;

extern int fscount;

/* A wrapper around malloc to keep track of dynamically allocated memory
 */
void *gc_malloc(int nbytes);

/* Executes the garbage collector.
 * mark_obj will traverse the data structure rooted at obj, calling
 * mark_one for each dynamically allocated chunk of memory to mark
 * that it is still in use.
 */
void mark_and_sweep(void *obj, void (*mark_obj)(void *));

/* Mark vptr as still in use
 * Return code:
 *   0 on success
 *   1 if memory chunk pointed to by vptr was already marked as in use
 */
int mark_one(void *vptr);

#endif
