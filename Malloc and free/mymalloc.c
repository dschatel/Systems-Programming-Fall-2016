#include <stdio.h>
#include "mymalloc.h"

static unsigned char memblock[MEMSIZE];

void initialize() {

	short int blocksize = ((sizeof(memblock) -2) << 1);
	memblock[0] = blocksize & 0xff;
	memblock[1] = (blocksize >> 8) & 0xff;

	int i = 2;

	while (i < sizeof(memblock)) {
		memblock[i] = '0';
		i++;
	}

}

void * my_malloc (size_t size, char * file, int line) {

	//if (memblock[0] == 0)
	//	initialize();

	short int meta_data = (size << 1) | 1;

	int i = 0;

	while (i <= sizeof(memblock)) {

		short int block_data = (int) memblock[i] + (int) (memblock[i+1] << 8);
		short int block_remainder = (((block_data ^1) >> 1) - size - 2) << 1;
		if ((block_data & 1) == 0) {
			if (((block_data ^ 1) >> 1) >= size+2) {
				memblock[i] = meta_data & 0xff;
				memblock[i+1] = (meta_data >> 8) & 0xff;
				int j = i + size + 2;
				memblock[j] = block_remainder & 0xff;
				memblock[j+1] = (block_remainder >> 8) & 0xff;
				return &memblock[i+2];

			}
		}

		i = i + ((block_data ^ 1) >> 1) + 2;

	}

	if (i >= sizeof(memblock)) {
		printf("Not enough space for allocation! Line %d of File %s\n", line, file);
	}

	return NULL;


}

void mergeBlocks () {

	int i = 0;

	//printBlockList();

	while (i < sizeof(memblock)) {
		short int block_data = (int) memblock[i] + (int) (memblock[i+1] << 8);
		if ((block_data & 1) == 0) {
			int j = i + ((block_data ^1) >> 1) + 2;
			short int block_data2 = (int) memblock[j] + (int) (memblock[j+1] << 8);

			if(j < sizeof(memblock) && (block_data2 & 1) == 0) {
				short int new_size = ((((block_data ^ 1) >> 1) + ((block_data2 ^1) >> 1)) + 2) << 1;
				memblock[i] = new_size & 0xff;
				memblock[i+1] = (new_size >> 8) & 0xff;
				memblock[j] = '0';
				memblock[j+1] = '0';
			}
			else
				i = i + ((block_data ^ 1) >> 1) + 2;

		}
		else
			i = i + ((block_data ^ 1) >> 1) + 2;


	}

}


void my_free (void * ptr, char * file, int line) {

	unsigned char * pointer = (unsigned char *) ptr;




	if (pointer == NULL) {
		printf("No pointer to free! Line %d of File %s\n", line, file);

	}
	else if (pointer < &memblock[0] || pointer > &memblock[MEMSIZE-1]) {
		printf("Address was not a pointer! Line %d of File %s\n", line, file);
	}

	else {
	short int meta = (int) pointer[-2] + (int) (pointer[-1] << 8);
	if ((meta & 1) != 1) {
		printf("Address was not allocated by malloc!Line %d of File %s\n", line, file);
	}
	else {
		meta = (meta ^ 1);
		pointer[-2] = meta & 0xff;
		pointer[-1] = (meta >> 8) & 0xff;
		mergeBlocks();
		}
	}



}

void printBlockList () {

	int i = 0;
	int counter = 0;

	while (i < sizeof(memblock)) {

		short int block_size = (int) memblock[i] + (int) (memblock[i+1] << 8);
		int alloc = block_size & 1;
		block_size = (block_size ^ 1) >> 1;
		printf("Block Location %d: %d Allocated: %d\n", i, block_size, alloc);
		counter++;
		i = i + block_size + 2;

	}

	printf("Total Allocations: %d\n", counter);
}

