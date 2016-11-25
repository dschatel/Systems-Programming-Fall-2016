#include <stdio.h>

#define MEMSIZE 5000

#define malloc(x) my_malloc( x, __FILE__, __LINE__ )
#define free(x) my_free( x, __FILE__, __LINE__ )

void initialize();

void * my_malloc (size_t size, char * file, int line);

void mergeBlocks ();

void my_free (void * ptr, char * file, int line);

void printBlockList ();
