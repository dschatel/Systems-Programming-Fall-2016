#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>

/*
 * This file should handle receiving the original file input,
 * forking processes, and passing the strings to be worked on to
 * those processes along with the filename to be used.
 *
 * It should pass the filename based on the current iteration of the loop.
 */

/*
 * compressR_LOLS(<file to compress>, <number of parts>)
 *
 * 1. Take in filename and number of parts
 * 2. Do arithmetic to determine length of each part
 * 3. Remainder should all be appended to first file (e.g. 11 / 3 = 5, 3, 3)
 * 4. Fork process
 * 5. Exec with following args: name of file to work on, start point in file, length of line, new filename
 * 6. At end of loop, new loop to have parent process wait until all children are done
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

		//strncpy(newname, oldname, i);
		//newname[i] = '\0';
		strcat(newname, "_LOLS");

		if(singlefile == 1) { //Multi-part

			char buffer[20];
			snprintf(buffer, 20, "%d", numappend);

			strcat(newname, buffer);
		}

	//printf("%s\n", newname);
	return newname;

}


//Handles checking length of the file, dividing into subparts, and forking/waiting
void compress_R_LOLS(char * filename, int parts) {

	//printf("Filename: %s, NumParts: %d\n", filename, parts);

	int file_length = readLength(filename);

	if (file_length == -1) {
		return;
	}
	else if(parts > file_length) {
		printf("ERROR: Requested parts greater than length of file\n");
		return;
	}

	//printf("Length of file: %d\n", file_length);
	//fflush(stdout);


	char * newFileName;

		if (parts == 1) {

			pid_t pid = getpid();
			newFileName = getfilename(filename, 0, parts);
			//fork to worker file: exec: filename, newFileName, 0, file_length
			pid = fork();

			if(pid == 0) {
								//printf("Running child process...\n");
								char * argv[6];
								argv[0] = "./compressR_worker_LOLS";
								argv[1] = filename;
								argv[2] = newFileName;
								char * partlengthChar[20];
								snprintf((char*) partlengthChar, 20, "%d", file_length);
								argv[3] = "0";
								argv[4] = (char *) partlengthChar;
								argv[5] = NULL;

								//printf("%s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);
								execvp(argv[0], argv);
							}


			//printf("Forking to Worker process with - Filename: %s, new Filename: %s, Read Start Loc: %d, Read Length: %d\n", filename, newFileName, 0, file_length);
		}
		else if (parts > 1) {
			int i;
			int partlength;
			int startingLoc = 0;
			pid_t pid = getpid();

			for (i = 0; i < parts; i++) {
				newFileName = getfilename(filename, 1, i);
				if (file_length % (parts-i) != 0)
					partlength = (file_length / (parts-i)) + (file_length % (parts - i));
				else
					partlength = file_length / (parts - i);

				//fork to worker file: exec: filename, newFileName, partlength, startingLoc

				pid = fork();

				if(pid == 0) {
					//printf("Running child process...\n");
					char * argv[6];
					argv[0] = "./compressR_worker_LOLS";
					argv[1] = filename;
					argv[2] = newFileName;
					char * startingLocChar[20];
					char * partlengthChar[20];
					snprintf((char*) startingLocChar, 20, "%d", startingLoc);
					snprintf((char *)partlengthChar, 20, "%d", partlength);
					argv[3] = (char*) startingLocChar;
					argv[4] = (char*) partlengthChar;
					argv[5] = NULL;

					//printf("%s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);
					execvp(argv[0], argv);
					break;
				}


				//printf("Forking to Worker process with - Filename: %s, new Filename: %s, Read Start Loc: %d, Read Length: %d\n", filename, newFileName, startingLoc, partlength);

				startingLoc+= partlength;
				file_length = file_length - partlength;
			}

		}
		else
			printf("Invalid parts argument\n");

		pid_t wpid;
		while((wpid = waitpid(-1, NULL, 0))) {
			if (errno == ECHILD)
				break;
				//printf("Waiting...\n");
		}
		//printf("Parent finished waiting.\n");


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
		printf("Usage: compressR_LOLS.c <file to compress> <number of parts>\n");
		return 0;
	}
	else {
		char * filename = argv[1];

		int val = atoi(argv[2]);

		compress_R_LOLS(filename, val);
	}

	return 0;

}
