#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "mymalloc.h"

void test_one () {

	initialize();
	char * ptr_array[sizeof(char *)*3000];


	int i = 0;
	//Malloc 3000 times
	for (; i < sizeof(ptr_array)/sizeof(char*); i=i+sizeof(char*)) {
		char * ptr = malloc(1);
		ptr_array[i] = ptr;
	}

	//Free 3000 times
	i = 0;
	for (; i < sizeof(ptr_array)/sizeof(char*); i=i+sizeof(char*)) {
		free(ptr_array[i]);
	}

}

void test_two() {

	initialize();
	//Malloc 1 byte and free it 3000 times
	char * p = malloc(1);

	int i = 0;
	for (; i < 3000; i++) {
		free(p);
	}

}

void test_three() {

	initialize();
	int counter = 0;
	int mallocs = 0;
	int frees = 0;
	time_t t;
	int ptr_loc = 0;
	char * ptr_array[sizeof(char *)*3000];

	srand((unsigned) time(&t));

	while (counter < 6000) {

		int random = rand() % 2;

		if(random == 0 && mallocs < 3000) {
			char * ptr = malloc(1);
			ptr_array[ptr_loc] = ptr;
			ptr_loc++;
			mallocs++;
			counter++;
		}
		else if (random == 1 && frees < 3000) {
			free(ptr_array[ptr_loc-1]);
			if (ptr_loc > 0)
				ptr_loc--;
			frees++;
			counter++;
		}
	}

}

void test_four () {

		initialize();
		int counter = 0;
		int mallocs = 0;
		int frees = 0;
		time_t t;
		int ptr_loc = 0;
		char * ptr_array[sizeof(char *)*3000];

		srand((unsigned) time(&t));

		while (counter < 6000) {

			int random = rand() % 2;

			if(random == 0 && mallocs < 3000) {
				int randSize = rand() % 5000;
				mallocs++;
				counter++;;
				char * ptr = malloc(randSize);
				ptr_array[ptr_loc] = ptr;
				ptr_loc++;
			}
			else if (random == 1 && frees < 3000) {
				frees++;
				counter++;
				free(ptr_array[(ptr_loc)-1]);
				if (ptr_loc > 0)
					ptr_loc--;
			}
		}

}

void test_five () {

	initialize();
	char * ptr_array[sizeof(char *)*5000];



	//Malloc 0 5000 times
	int i = 0;
	for (; i < sizeof(ptr_array)/sizeof(char*); i=i+sizeof(char*)) {
		char * ptr = malloc(0);
		ptr_array[i] = ptr;
	}
	i = 0;
	//Free 0 5000 times
	for (; i < sizeof(ptr_array)/sizeof(char*); i=i+sizeof(char*)) {
		free(ptr_array[i]);
	}

}

void test_six () {

	initialize();
	char * ptr_array[sizeof(char *)*3000];



	//Malloc 10000 3000 times
	int i = 0;
	for (; i < sizeof(ptr_array)/sizeof(char*); i=i+sizeof(char*)) {
		char * ptr = malloc(10000);
		ptr_array[i] = ptr;
	}

	//Free 10000 3000 times
	i = 0;
	for (; i < sizeof(ptr_array)/sizeof(char*); i=i+sizeof(char*)) {
		free(ptr_array[i]);
	}

}

float workload(int test) {

	initialize();

	int workload = 0;
	int totalTime = 0;

	while (workload < 100) {
		struct timeval start, end;
		gettimeofday(&start, NULL);
		switch(test) {
		case 1: test_one();
				break;
		case 2: test_two();
				break;
		case 3: test_three();
				break;
		case 4: test_four();
				break;
		case 5: test_five();
				break;
		case 6: test_six();
				break;
		}
		gettimeofday(&end, NULL);
		totalTime = totalTime + ((end.tv_sec * 1000000 + end.tv_usec)
				- (start.tv_sec * 1000000 + start.tv_usec));
		workload++;

	}

	long int timeAverage = (totalTime / 100);
	float timeInSecs = (float) timeAverage / 1000000;

	return timeInSecs;

}

int main() {

	//float testOne = workload(1);
	//float testTwo = workload(2);
	//float testThree = workload(3);
	float testFour = workload(4);
	//float testFive = workload(5);
	//float testSix = workload(6);

	//printf("Average Time for Test One: %f seconds\n", testOne);
	//printf("Average Time for Test Two: %f seconds\n", testTwo);
	//printf("Average Time for Test Three: %f seconds\n", testThree);
	printf("Average Time for Test Four: %f seconds\n", testFour);
	//printf("Average Time for Test Five: %f seconds\n", testFive);
	//printf("Average Time for Test Six: %f seconds\n", testSix);
	printBlockList();

	return 0;

}
