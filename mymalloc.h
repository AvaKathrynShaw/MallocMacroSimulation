#ifndef _mymalloc_h_
#define _mymalloc_h_

#include <stdlib.h>
#include <stdio.h>

#define malloc( x ) myMalloc( x , __FILE__ , __LINE__ )
#define free( x ) myFree( x , __FILE__ , __LINE__ )

#define MEMSIZE 50000

//the stucture of the memory entry per class lecture and book
struct memoryEntry
{
	//pointer to previous and next blocks in memory
	struct memoryEntry *prev, *next;
	//works a a boolean
	// 1 - yes, 0 - no
	int isfree;	
	// holds the size of the block	
	int size;
};

void *myMalloc(unsigned int size, char *file, int line);
void myFree(void *p, char *file, int line);

#endif
