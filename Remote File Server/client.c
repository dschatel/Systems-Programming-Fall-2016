#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include "libnetfiles.h"

int main (int argc, char ** argv) {

	char buffer[256];

	printf("Please enter a hostname: ");
	char host[256];
	fgets(buffer, 256, stdin);
	strcpy(host, buffer);
	size_t ln = strlen(host)-1;
	if (host[ln] == '\n')
	    host[ln] = '\0';


	printf("\nPlease enter a mode (0 = unrestricted, 1 = exclusive, 2 = transactional): ");
	bzero(buffer, 256);
	fgets(buffer, 256, stdin);
	int mode = atoi(buffer);

	printf("\nHost: %s Mode: %d\n", host, mode);

	int res = netserverinit(host, mode);

	if (res == -1) {
		return -1;
	}

	int choice = 0;

	while (choice != 5) {

		printf("\nPlease select an option:\n1. Open file\n2. Read file\n3. Write file\n4. Close file\n5. Quit\nEnter choice >> ");
		bzero(buffer, 256);
		fgets(buffer, 256, stdin);
		choice = atoi(buffer);


		if (choice == 1) {

			char pathname[256];
			int flag;

			printf("\nEnter a pathname: ");
			bzero(buffer, 256);
			fgets(buffer, 256, stdin);
			strcpy(pathname, buffer);
			size_t ln = strlen(pathname)-1;
			if (pathname[ln] == '\n')
			    pathname[ln] = '\0';


			printf("\nEnter permissions (O_RDONLY = 0, O_WRONLY = 1, O_RDWR = 2): ");
			bzero(buffer, 256);
			fgets(buffer, 256, stdin);
			flag = atoi(buffer);

			res = netopen(pathname, flag);

			if (res != -1 && res != 1 && res != 2)
				printf("\nYour file descriptor: %d", res);
			else
				printf("\nUnable to open file. Try again.");

		}
		else if (choice == 2) {
			int fd;
			char buffer[256];
			int bytes;

			printf("\nEnter file descriptor: ");
			bzero(buffer, 256);
			fgets(buffer, 256, stdin);
			fd = atoi(buffer);

			printf("\nEnter number of bytes to read (max 256): ");
			bzero(buffer, 256);
			fgets(buffer, 256, stdin);
			bytes = atoi(buffer);

			res = netread(fd, &buffer, bytes);

			if (res > 0)
				printf("\nYou read: %s", buffer);
			else
				printf("\nUnable to read file. Try again.");
		}
		else if (choice == 3) {
			int fd;
			char buffer[256];
			int bytes;
			char to_write[256];

			printf("\nEnter file descriptor: ");
			bzero(buffer, 256);
			fgets(buffer, 256, stdin);
			fd = atoi(buffer);

			printf("\nEnter text to write (max 256 bytes): ");
			bzero(buffer, 256);
			fgets(buffer, 256, stdin);
			strcpy(to_write, buffer);
			size_t ln = strlen(to_write)-1;
			if (to_write[ln] == '\n')
				to_write[ln] = '\0';

			printf("\nEnter number of bytes to read (max 256): ");
			bzero(buffer, 256);
			fgets(buffer, 256, stdin);
			bytes = atoi(buffer);

			res = netwrite(fd, &to_write, bytes);

			if (res > 0)
				printf("\nYou wrote: %s", to_write);
			else
				printf("\nUnable to write to file. Try again.");

		}
		else if (choice == 4) {

			int fd;

			printf("\nEnter file descriptor: ");
			bzero(buffer, 256);
			fgets(buffer, 256, stdin);
			fd = atoi(buffer);

			res = netclose(fd);

			if (res == 0)
				printf("\nFile close successful.");
			else
				printf("\nCould not close file.");

		}

	}

	return 0;

}
