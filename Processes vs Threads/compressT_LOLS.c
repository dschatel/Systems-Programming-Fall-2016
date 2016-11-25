#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

//Struct to hold all data to be passed to function
typedef struct _compress {
	char * filename;
	char * newFileName;
	int startingLoc;
	int fileLength;

} CompressVals;



/*
 * compressT_LOLS(<file to compress>, <number of parts>)
 *
 * 1. Take in filename and number of parts
 * 2. Do arithmetic to determine length of each part
 * 3. Remainder should all be appended to first file (e.g. 11 / 3 = 5, 3, 3)
 * 4. Create struct to hold data passed to thread
 * 5. Create a thread
 * 6. At end of loop, new loop to join all threads
 */

//Reads the character length of the file. If the file does not exist or the user
//Lacks read access, reports an error and returns -1.
int readLength (char * filename) {

	int file_length = 0;
	int rval;

	rval = access(filename, F_OK);

	if (rval == 0) {}
	else if (errno == ENOENT) {
		printf("ERROR: File does not exist.");
		return -1;
	}
	else if (errno == EACCES) {
		printf("ERROR: File is not accessible.");
		return -1;
	}

	rval = access(filename, R_OK);

	if (rval == 0) {}

	else {
		printf("ERROR: No read access to file.");
		return -1;
	}



	FILE * input_file = fopen(filename, "r");

	if(input_file) {
		fseek(input_file, 0, SEEK_END);
		file_length = ftell(input_file);
		fclose(input_file);
	}
	else
		file_length = -1;

	return file_length;
}

//Generates the filename to be passed to the worker file for output
char * getfilename (char * oldname, int singlefile, int numappend) {

	char * newname = malloc(sizeof(char) * (strlen(oldname)+1));

	strcpy(newname, oldname);

		int i;
		for (i = 0; i < strlen(newname); i++) {
			if ((char) newname[i] == '.')
				newname[i] = '_';
		}

		strcat(newname, "_LOLS");

		if(singlefile == 1) { //Multi-part

			char buffer[20];
			snprintf(buffer, 20, "%d", numappend);

			strcat(newname, buffer);
		}

	return newname;

}


//Runs LOLS compression algorithm and outputs to file with specified name
void * compress(void * val) {

	CompressVals * vals = val;

	char * filename = vals -> filename;
	char * newName = vals -> newFileName;

	int startLoc = vals-> startingLoc;
	int fileLength = vals -> fileLength;

	//printf("Running Thread: Filename: %s, new Filename: %s, Read Start Loc: %d, Read Length: %d\n", filename, newName, startLoc, fileLength);

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
	free(vals);

	return NULL;



}

//Handles checking length of the file, dividing into subparts, and creating/joining threads
void compress_T_LOLS(char * filename, int parts) {

	//printf("Filename: %s, NumParts: %d\n", filename, parts);

	int file_length = readLength(filename);
	pthread_t threads[parts];
	void * result[parts];

	if (file_length == -1) {
		return;
	}
	else if(parts > file_length) {
		printf("ERROR: Requested parts greater than length of file.\n");
		return;
	}

	//printf("Length of file: %d\n", file_length);
	//fflush(stdout);


	char * newFileName;

		if (parts == 1) {
			newFileName = getfilename(filename, 0, parts);

			CompressVals * val = malloc(sizeof(CompressVals));
			val -> filename = filename;
			val -> newFileName = newFileName;
			val -> startingLoc = 0;
			val -> fileLength = file_length;

			pthread_create(&threads[0], NULL, compress, val);
			}

		else if (parts > 1) {
			int i;
			int partlength;
			int startingLoc = 0;

			for (i = 0; i < parts; i++) {
				newFileName = getfilename(filename, 1, i);
				if (file_length % (parts-i) != 0)
					partlength = (file_length / (parts-i)) + (file_length % (parts - i));
				else
					partlength = file_length / (parts - i);

				CompressVals * val = malloc(sizeof(CompressVals));
				val -> filename = filename;
				val -> newFileName = newFileName;
				val -> startingLoc = startingLoc;
				val -> fileLength = partlength;

				pthread_create(&threads[i], NULL, compress, val);

				startingLoc+= partlength;
				file_length = file_length - partlength;
			}

		}
		else
			printf("Invalid parts argument\n");

			int i;
		  for (i = 0; i < parts; i++)
		    {
		      if (pthread_join(threads[i], &result[i]) == 0)
		        {
		    	  //printf("Joining thread...\n");
		        }
		    }

		  //printf("Finished joining threads.\n");

	}

int main (int argc, char ** argv) {

	//printf("Starting program...\n");

	if (argc > 3) {
		fprintf(stderr, "ERROR: Too many arguments.\n");
		return 1;
	}
	else if (argc < 3) {
		fprintf(stderr, "ERROR: Too few arguments.\n");
		return 1;
	}

	if (strcmp(argv[1], "-h") == 0) {
		printf("Usage: compressT_LOLS.c <file to compress> <number of parts>\n");
		return 0;
	}
	else {
		char * filename = argv[1];

		int val = atoi(argv[2]);

		compress_T_LOLS(filename, val);
	}

	return 0;

}
