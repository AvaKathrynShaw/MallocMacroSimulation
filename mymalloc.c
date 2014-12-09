#include "mymalloc.h"



// ***** Defining Block of Memory to memoryEntries to be stored in ******
//creates a big block of memory space
static char bigBlock[MEMSIZE];
 //creates the size of the memory entries
static const int sizeOfEntry = MEMSIZE/sizeof(struct memoryEntry)+1;
// creates typeless pointer to memoryEntries
static void *memoryEntries[MEMSIZE/sizeof(struct memoryEntry)+1] = {0};
//***********************************************************************

// return the first occurance of an index not containing a memoryEntry
static int findNextIndex() {
	int i;
	//loop until we get to empty space in large block of memory array
	for (i = 0; i < sizeOfEntry; i++)
		if (memoryEntries[i] == 0) 
			return i;
	return 1; //should never reach here but 0 is always set as root
}

// return a pointer to the memory buffer requested
void *myMalloc(unsigned int size, char *file, int line)
{
	//holds index returned from calling findNextIndex
	int i;
	//used as a boolean
	// 1 - yes, 0 - no
	static int initialized = 0;
	//pointer to a memory entry
	struct memoryEntry *p;
	//iterator pointing to the next entry
	struct memoryEntry *next;
	//create static entry of the root
	//this should never need to be defragmented so we should not need to move it
	static struct memoryEntry *root;
	
	//check that there is at least any space
	if (size == 0) {
		//print error if there is not
		fprintf(stderr, "OUT OF MEMORY IN FILE: '%s' ON LINE: '%d'\n", file, line);
		//get out of here!
		return 0;
	}

	// 1st time called we must initialize the block of memory
	if(!initialized)	
	{
		// Initialize a root chunk at the beginning of the memory block

		//cast the char array to a bigBlock type
		root = (struct memoryEntry*) bigBlock;
		//set the pointers to the other blocks to 0
		root->prev = root->next = 0;
		//reduce the size of the memory block by the size of the memory entry
		root->size = MEMSIZE - sizeof(struct memoryEntry);
		//set is free to yes, no data has been entered to it but...
		root->isfree = 1;
		//we did initialize it so set this boolean to 1
		initialized = 1;
		//take that block and place it into memory entried big block of memory
		i=findNextIndex();
		memoryEntries[i] = bigBlock;
	}
	//set pointer to the beginning of the big block of memory
	p = root;
	do
	{
		// the current chunk is smaller, go to the next chunk
		if(p->size < size || !p->isfree) {
			// or this chunk was taken, go to the next
			p = p->next;
		}
		// this chunk is free and large enough to hold data,
		else if(p->size < (size + sizeof(struct memoryEntry))) {
			// but there's not enough memory to hold the HEADER of the next chunk
			// don't create any more chunks after this one
			p->isfree = 0;
			return (char*)p + sizeof(struct memoryEntry);
		}
		else {
			// take the needed memory and create the header of the next chunk
			next = (struct memoryEntry*)((char*)p + sizeof(struct memoryEntry) + size);
			//the the previous pointer to the root
			next->prev = p;
			//set the next pointer to the next block after the root
			next->next = p->next;
			//deduct the size of the allocated memory
			next->size = p->size - sizeof(struct memoryEntry) - size;
			//set the next block to free being true
			next->isfree = 1;
			//we take the next free index of the big block of memory 
			i=findNextIndex();
			memoryEntries[i] = next;
			//set the size of the current to size
			p->size = size;
			//it is not longer free, change boolean value
			p->isfree = 0;
			//set current block pointer of next to the header of the next chunk of memory
			p->next = next;
			//cast it back to a char type and return the new block of memory plus the size of structure
			return (char*)p + sizeof(struct memoryEntry);
		}
	} while (p != 0);


	fprintf(stderr, "UNABLE TO ALLOCATE (%d bytes) in FILE: '%s' on LINE: '%d'\n", size, file, line);
	return 0;
}


// free a memory buffer pointed to by p
void myFree(void *p, char *file, int line)
{
	struct memoryEntry *ptr;
	struct memoryEntry *prev;
	struct memoryEntry *next;


	//if what was passed in was null print an error message
	if (p == NULL) {
		fprintf(stderr, "ERROR: NULL POINTER PASSED IN: CANNOT FREE at FILE: '%s' on LINE: '%d'\n", file, line);
		return;
	}
	//else cast passed in as void entry p to a char* less the size of the memoryEntry structure and cast that object to the memory entry
	ptr = (struct memoryEntry*)((char*)p - sizeof(struct memoryEntry));
	
	//check if valid memoryEntry ptr
	int i;
	int valid = 0;
	for (i = 0; i < sizeOfEntry; i++) {
		if (ptr == memoryEntries[i] && !ptr->isfree) {
			valid = 1; //memoryEntry is valid
			break;
		}
	}//if the pointer passed in was not malloc()ed AKA is not valid
	if (!valid) {
		//print error message and exit
		fprintf(stderr, "ERROR: DID NOT MALLOC THIS OBJECT: CANNOT FREE in FILE: '%s' on LINE: '%d'\n", file, line);
		return;
	}
	//if the previous chunk of memory is free
	if((prev = ptr->prev) != 0 && prev->isfree)
	{
		// merge this chunk with the previous chunk to not allow fragmentation
		prev->size += sizeof(struct memoryEntry) + ptr->size;
		//merged with previous, so removing the freed memoryEntry
		memoryEntries[i] = 0; 
	}
	else
	{ //not setting memoryEntry to null b/c not necessarily removing it, just isfree=1
		ptr->isfree = 1;
		prev = ptr;	// used for the step below
	}//if the next chunk is free
	if((next = ptr->next) != 0 && next->isfree)
	{
		// merge with it to never allow defragmentation
		prev->size += sizeof(struct memoryEntry) + next->size;
		prev->next = next->next;
		for (i = 0; i < sizeOfEntry; i++) {
			if (next == memoryEntries[i]) {
				memoryEntries[i] = 0; //merged with next, so removing free memoryEntry
				break;
			}
		}
	}


}
int main(int argc, char const *argv[])
{
	    //TEST CASE 1
        printf("TEST 1: MALLOCING A NULL OBJECT\n");
        malloc(0);

        //TEST CASE 2
        printf("TEST 2: MALLOCING A VOID* SIZE 1000\n");
        void *p1 = malloc(1000);
        printf("p1 1000 '%p'\n", p1);

        //TEST CASE 3
        printf("TEST 3: MALLOC POINTER SIZE 20000 AND FREEING\n");
        void *p4 = malloc(20000);
        printf("p4 20000 '%p'\n", p4);
        printf("Freeing p4\n");
        free(p4);

       //TEST CASE 4
         printf("TEST 4: DOUBLE FREE SHENANIGANS\n");
            void* p = malloc(1);
            free(p);
            free(p);

        //TEST CASE 5
        printf("TEST 5: FREE A NULL POINTER\n");
        free(NULL);

        //TEST CASE 6
        printf("TEST 6: FREE MEMORY NOT MALLOCED\n");
            int x;
    		free(&x);

        //TEST CASE 7
        void* p2;
        int i;
        printf("TEST 7: ALLOCATING UNTIL WE RUN OUT OF SPACE\n");
        for(i = 1; (p2 = malloc(i % 2 == 0 ? 1 : 30)) != NULL; i++){
                printf("MEMORY ENTRIES ALLOCATED: %d\n", i);
        }
	return 0;
}
