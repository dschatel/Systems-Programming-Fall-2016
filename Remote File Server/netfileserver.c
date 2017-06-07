#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

#define PORT_NO 42069

typedef struct _newsock {
	int newsockfd;

} newsock_container;

typedef struct _fdstruct {
	int networkFD;
	int realFD;
	char * filepath;
	int fileMode;
	int permissions;
	struct _fdstruct * next;
} FDStruct;

FDStruct * head;
pthread_mutex_t lock;
pthread_mutex_t rw_lock;

int find_real_fd (int networkFD) {

	FDStruct * ptr = head;

	while (ptr != NULL) {
		if (ptr->networkFD == networkFD)
			break;
		ptr = ptr->next;
	}

	if (ptr != NULL)
		return ptr->realFD;
	else
		return -1;

}

void deleteNode (int networkFD) {

	FDStruct * ptr = head;
	FDStruct * prev = ptr;

	while (ptr != NULL) {
		if(ptr->networkFD == networkFD)
			break;

		prev = ptr;
		ptr = ptr->next;
	}

	prev->next = ptr->next;
	if (ptr == head)
		head = head->next;

	free(ptr);

}

char * server_read(char * buffer) {

	char arg1[2];
	char arg2[100];
	char arg3[5000];

	strcpy(arg1, strtok(buffer, ";"));
	strcpy(arg2, strtok(NULL, "\0"));

	int networkFD = atoi(arg1);
	int bytes_to_read = atoi(arg2);

	printf("File Descriptor: %d, Bytes to Read: %d\n", networkFD, bytes_to_read);

	pthread_mutex_lock(&lock);
	int fildes = find_real_fd(networkFD);
	pthread_mutex_unlock(&lock);

	//lseek(fildes, 0, SEEK_SET);
	pthread_mutex_lock(&rw_lock);
	int bytes_read = read(fildes, arg3, bytes_to_read);
	pthread_mutex_unlock(&rw_lock);

	int error = 0;
	if (bytes_read < 0) {
		error = errno;
	}

	bzero(buffer, 256);
	snprintf(buffer, 256, "%d;%d;%s", bytes_read, error, arg3);
	printf("%s\n", buffer);

	return buffer;


}

char * server_write(char * buffer) {

	char arg1[2];
	char arg2[100];
	char arg3[5000];

	strcpy(arg1, strtok(buffer, ";"));
	strcpy(arg2, strtok(NULL, ";"));
	strcpy(arg3, strtok(NULL, "\0"));

	int networkFD = atoi(arg1);
	int bytes_to_write = atoi(arg2);

	printf("File Descriptor: %d, Bytes to Read: %d\n", networkFD, bytes_to_write);

	pthread_mutex_lock(&lock);
	int fildes = find_real_fd(networkFD);
	pthread_mutex_unlock(&lock);

	pthread_mutex_lock(&rw_lock);
	int bytes_written = write(fildes, arg3, bytes_to_write);
	pthread_mutex_unlock(&rw_lock);

	int error = 0;
	if (bytes_written < 0) {
		error = errno;
	}

	bzero(buffer, 256);
	snprintf(buffer, 256, "%d;%d", bytes_written, error);
	printf("%s\n", buffer);

	return buffer;


}


char * server_open(char * buffer) {
	char pathname[256];
	char flag[2];
	char mode[2];

	errno = 0;

	strcpy(mode, strtok(buffer, ";"));
	strcpy(flag, strtok(NULL, ";"));
	strcpy(pathname, strtok(NULL, "\0"));


		int flags = atoi(flag);
		int open_mode = atoi(mode);

		FDStruct * ptr = head;
		int open_allowed = 0;

		pthread_mutex_lock(&lock);
		while(ptr != NULL) {

			printf("Checking list of open files...\n");

			if (strcasecmp(pathname, ptr->filepath) == 0) {
				printf("File is already open!\n");
				if (open_mode == 0) { //Unrestricted open requested
					printf("Unrestricted mode requested...\n");
					if (ptr->fileMode == 2) // Entry already opened in transactional
						open_allowed = 2; //Disallowed unrestricted open, already opened in transactional
					else if (flags != O_RDONLY) { //Requested unrestricted write
						if (ptr->fileMode == 1 && ptr->permissions != O_RDONLY) //Entry opened in exclusive write
							open_allowed = 1; //Disallowed unrestricted write, already opened in exclusive write
					}
				}
				else if (open_mode == 1) { //exclusive open requested
					printf("Exclusive mode requested...\n");
					printf("Flags is %d\n", flags);
					if (ptr->fileMode == 2) // Entry already opened in transactional
						open_allowed = 2; //Disallowed exclusive open, already opened in transactional
					else if (flags != O_RDONLY) { //Requested exclusive write
						printf("Exclusive write requested...\n");
						if (ptr->permissions != O_RDONLY) //Entry opened for writing
							printf("Exclusive write already exists!\n");
							open_allowed = 1; //Disallowed unrestricted write, already opened in write mode elsewhere
					}
				}
				else if (open_mode == 2) { //Transactional open requested
					printf("Transactional mode requested...\n");
					open_allowed = 2; //Pathname exists, so already open elsewhere. Transactional open disallowed
				}
			}

			ptr = ptr->next;

		}
		pthread_mutex_unlock(&lock);

		int res;
		int networkFD;

		if (open_allowed == 0) {
			res = open(pathname, flags);

			pthread_mutex_lock(&lock);
			if (res != -1) {
				FDStruct * newFD = malloc(sizeof(FDStruct));
				newFD->networkFD = res * -1;
				networkFD = newFD-> networkFD;
				newFD->realFD = res;
				newFD->filepath = malloc(sizeof(char)*strlen(pathname));
				strcpy(newFD->filepath, pathname);
				newFD->fileMode = open_mode;
				newFD->permissions = flags;
				newFD->next = head;
				head = newFD;
			}
			else if (res == -1)
				networkFD = res;
			pthread_mutex_unlock(&lock);
		}
		else
			networkFD = open_allowed;

		bzero(buffer, 256);
		snprintf(buffer, 256, "%d;%d", networkFD, errno);

		return buffer;

}

char * server_close(char * buffer) {

	char fildes[10];
	errno = 0;

	strcpy(fildes, strtok(buffer, "\0"));

	int networkFD = atoi(fildes);

	pthread_mutex_lock(&lock);
	int fd = find_real_fd(networkFD);
	pthread_mutex_unlock(&lock);

	int res = close(fd);

	pthread_mutex_lock(&lock);
	if (res != -1)
		deleteNode(networkFD);
	pthread_mutex_unlock(&lock);

	bzero(buffer, 20);
	snprintf(buffer, 20, "%d;%d", res, errno);

	return buffer;

}

void * new_thread (void * newsocks) {

	newsock_container * newsock = newsocks;

	int newsockfd = newsock->newsockfd;

	char buffer[256];
	int n = read(newsockfd, buffer, sizeof(buffer));

	buffer[n] = '\0';

	printf("Bytes read: %d\n", n);

	if (n < 0)
		printf("Error reading from socket");

	printf("%s\n", buffer);



	char server_command[2];
	char rest[256];

	strcpy(server_command, strtok(buffer, ";"));
	strcpy(rest, strtok(NULL, "\0"));

	int command = atoi(server_command);

	char result[5000];
	bzero(result, 5000);


	switch(command) {
		case 1:
				printf("Opening...\n");
				strcpy(result, server_open(rest));
				break;
		case 2:
				printf("Reading...\n");
				strcpy(result, server_read(rest));
				break;
		case 3:
				printf("Writing...\n");
				strcpy(result, server_write(rest));
				break;
		case 4:
				printf("Closing...\n");
				strcpy(result, server_close(rest));
				break;
	}

	printf("Sending result: %s\n", result);
	n = write(newsockfd, result, 5000);


    if (n < 0) printf("ERROR writing to socket");
    close(newsockfd);
    return NULL;

}


int main(int argc, char ** argv) {

	int sockfd, newsockfd;
	socklen_t addr_size;
	struct sockaddr_in serv_addr, cli_addr;
	pthread_t tid;
	head = NULL;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("Creating socket...\n");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT_NO);

	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		printf("Error on binding\n");

	printf("Binding socket...\n");

	listen(sockfd, 5);

	printf("Listening...\n");

	while (1) {

		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &addr_size);
		printf("Connected!\n");

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		newsock_container * newsock = malloc(sizeof(newsock_container));
		newsock->newsockfd = newsockfd;

		if (pthread_create(&tid, &attr, new_thread, newsock) != 0) {
			printf("Error creating thread.\n");
		}

	}

	return 0;
}
