#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


//Runs LOLS compression algorithm and outputs to file with specified name
void compressR_LOLS(char* filename, char* newName, int startLoc, int fileLength) {

	//int pid = getppid();

	FILE * input_file = fopen(filename, "r");
	fseek(input_file, startLoc, SEEK_SET);
	FILE * output_file = fopen(newName, "w");

    int count;
    int ch, ch2;
    int i = 0;

    ch = getc(input_file);
    ch2 = ch;
    while (ch2 != EOF && i < fileLength) {
        // if next letter is the same increase count and test
        for (count = 0; ch2 == ch && count < 255 && i < fileLength; count++, i++) {
        	ch2 = getc(input_file);// set next variable for comparison
        }
        // write letters into new file
        if (count == 1)
            putc(ch, output_file);
        else if (count == 2) {
            putc(ch, output_file);
            putc(ch, output_file);
        }
        else {
        	fprintf(output_file, "%d", count);
            putc(ch, output_file);
        }
        ch = ch2;
    }

	fclose(input_file);
	fclose(output_file);


	//printf("Running from Process with Parent PID: %d; Filename: %s, new Filename: %s, Read Start Loc: %d, Read Length: %d\n", pid, filename, newName, startLoc, fileLength);



}


int main (int argc, char ** argv) {

	//pid_t pid = getppid();

	int startingLoc = atoi(argv[3]);
	int fileLength = atoi(argv[4]);

	compressR_LOLS(argv[1], argv[2], startingLoc, fileLength);

	//printf("Running from Process with Parent PID: %d; Filename: %s, new Filename: %s, Read Start Loc: %s, Read Length: %s\n", pid, argv[1], argv[2], argv[3], argv[4]);

	return 0;

}

