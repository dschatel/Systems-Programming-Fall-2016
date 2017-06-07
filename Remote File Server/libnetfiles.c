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
#include <arpa/inet.h>
#include "libnetfiles.h"

struct addrinfo *h, *servinfo;
int client_mode;

int netserverinit(char * hostname, int mode) {

	struct addrinfo hints, *p;
	int rv;

	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, PORT_NO, &hints, &servinfo)) != 0)
	{
		printf("ERROR: Host not found.");
		h_errno = HOST_NOT_FOUND;
		return -1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		h = p;

	}

	if (mode >= 0 && mode < 3)
		client_mode = mode;
	else {
		errno = INVALID_FILE_MODE;
		printf("Invalid file mode.\n");
		return -1;
	}



	return 0;
}

ssize_t netwrite(int fildes, const void *buf, size_t nbytes) {

	if(errno == INVALID_FILE_MODE) {
		printf("ERROR: Cannot write to file in this mode.\n");
		return -1;
	}

	if (h_errno == HOST_NOT_FOUND) {
		printf("ERROR: Host wasn't found, can't connect.");
	}

	errno = 0;

	char buffer[5000];

	snprintf(buffer, 5000, "%d;%d;%d;%s", 3, fildes, (int) nbytes, (char *) buf);

	//printf("%s\n", buffer);

	int sockfd = socket(h->ai_family, h->ai_socktype, h->ai_protocol);

	//printf("Writing over sockfd: %d\n", sockfd);

	if (connect(sockfd, h->ai_addr, h->ai_addrlen) < 0) {
		printf("Error connecting.\n");
	}

	int n = write(sockfd, buffer, strlen(buffer));

    bzero(buffer, 256);
    n = read(sockfd,buffer,255);
       if (n < 0)
            printf("ERROR reading from socket");

      //printf("%s\n", buffer);

      int bytes_written = atoi(strtok(buffer, ";"));
      int error = atoi(strtok(NULL, "\0"));

       if (error == EBADF)
    	   printf("ERROR: Bad file number.\n");
       else if (error == ETIMEDOUT)
    	   printf("ERROR: Connection timed out.\n");
       else if (error == ECONNRESET)
    	   printf("ERROR: Connection reset by peer.\n");

    close(sockfd);

	return bytes_written;
}

ssize_t netread(int fildes, void * buf, size_t nbytes) {

	if(errno == INVALID_FILE_MODE) {
		printf("ERROR: Cannot read file in this mode.\n");
		return -1;
	}

	if (h_errno == HOST_NOT_FOUND) {
		printf("ERROR: Host wasn't found, can't connect.");
	}

	errno = 0;

	char buffer[5000];

	snprintf(buffer, 5000, "%d;%d;%d", 2, fildes, (int) nbytes);

	//printf("%s\n", buffer);

	int sockfd = socket(h->ai_family, h->ai_socktype, h->ai_protocol);

	//printf("Reading over sockfd: %d\n", sockfd);

	if (connect(sockfd, h->ai_addr, h->ai_addrlen) < 0) {
		printf("Error connecting.\n");
	}

	int n = write(sockfd, buffer, strlen(buffer));

    bzero(buffer, 256);
    n = read(sockfd,buffer,255);
       if (n < 0)
            printf("ERROR reading from socket");

      // printf("%s\n", buffer);

   int bytes_read = atoi(strtok(buffer, ";"));
   int error = atoi(strtok(NULL, ";"));

  // printf("Value of n: %d", n);
   //printf("Bytes read: %d Error Code %d\n", bytes_read, error);

   if(error == 0 && bytes_read != 0)
	   strcpy(buf, strtok(NULL, "\0"));
   else if (error == EBADF)
	   printf("ERROR: Bad file number.\n");
   else if (error == ETIMEDOUT)
	   printf("ERROR: Connection timed out.\n");
   else if (error == ECONNRESET)
	   printf("ERROR: Connection reset by peer.\n");


   close(sockfd);

   return bytes_read;


}

int netopen(const char * pathname, int flags) {

	if(errno == INVALID_FILE_MODE) {
		printf("ERROR: Cannot open file in this mode.\n");
		return -1;
	}

	if (h_errno == HOST_NOT_FOUND) {
		printf("ERROR: Host wasn't found, can't connect.");
	}

	errno = 0;

	int size = (sizeof(char) * strlen(pathname)) + 7;

	char * buf = malloc(size);

	snprintf(buf, size, "%d;%d;%d;%s", 1, client_mode, flags, pathname);


	int sockfd = socket(h->ai_family, h->ai_socktype, h->ai_protocol);

	//printf("Opening over sockfd: %d\n", sockfd);

	if (connect(sockfd, h->ai_addr, h->ai_addrlen) < 0) {
		printf("Error connecting.\n");
	}

	int n = write(sockfd, buf, strlen(buf));
	//printf("Bytes sent: %d\n", n);
    if (n < 0)
         printf("ERROR writing to socket\n");

    bzero(buf,256);
    n = read(sockfd,buf,255);
       if (n < 0)
            printf("ERROR reading from socket");


    int fd = atoi(strtok(buf, ";"));
    int error = atoi(strtok(NULL, "\0"));

    //printf("File Descriptor: %d, ErrNo: %d\n", fd, error);

    if (fd == -1) {
    	if (error == EACCES)
    		printf("ERROR: Invalid permissions\n");
    	else if (error == ENOENT)
    		printf("ERROR: No such file or directory.\n");
    	else if (error == EINTR)
    		printf("ERROR: Interrupted system call.\n");
    	else if (error == EISDIR)
    		printf("ERROR: Path is a directory.\n");
    	else if(error == EROFS)
    		printf("ERROR: File system is read-only.\n");
    	else if(error == ENFILE)
    		printf("ERROR: File table overflow.\n");
    	else if(error == EWOULDBLOCK)
    		printf("ERROR: Operation would block.\n");
    	else if (error == EPERM)
    		printf("ERROR: Operation not permitted.\n");
    }
    else if (fd == 1) {
    	printf("ERROR: Cannot open file in requested mode. File already opened for writing elsewhere.\n");
    	fd = -1;
    }
    else if (fd == 2) {
    	printf("ERROR: Cannot open file in requested mode. File already opened in transactional mode by another user.\n");
    	fd = -1;
    }

    close(sockfd);



	return fd;

}

int netclose(int fd) {

	if(errno == INVALID_FILE_MODE) {
		printf("ERROR: Cannot close file in this mode.\n");
		return -1;
	}

	if (h_errno == HOST_NOT_FOUND) {
		printf("ERROR: Host wasn't found, can't connect.");
	}

	errno = 0;

	char buf[20];

	snprintf(buf, 20, "%d;%d", 4, fd);

	//printf("%s\n", buf);

	int sockfd = socket(h->ai_family, h->ai_socktype, h->ai_protocol);

	//printf("Closing over sockfd: %d\n", sockfd);

		if (connect(sockfd, h->ai_addr, h->ai_addrlen) < 0) {
			printf("Error connecting.\n");
		}

		int n = write(sockfd, buf, strlen(buf));

	    bzero(buf, 20);
	    n = read(sockfd,buf,20);
	       if (n < 0)
	            printf("ERROR reading from socket");

	       //printf("%s\n", buf);

	   int return_val = atoi(strtok(buf, ";"));
	   int error = atoi(strtok(NULL, "\0"));


	   if (error == EBADF)
		   printf("ERROR: Bad file number.\n");


	   close(sockfd);

	   //printf("Closing file.\n");

	   return return_val;


}
