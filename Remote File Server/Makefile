#Variable definition
CC  = gcc 
CCFLAGS = -Wall -g

#rules definition
all : netfileserver libnetfiles client

netfileserver : netfileserver.c
	$(CC) $(CCFLAGS) -pthread -o netfileserver netfileserver.c

libnetfiles : libnetfiles.c libnetfiles.h
	$(CC) $(CCFLAGS) -c -pthread -o libnetfiles libnetfiles.c
	
client: client.c libnetfiles.h
	$(CC) $(CCFLAGS) -pthread -o client client.c libnetfiles.c

#This rule cleans up executable file
clean: 
	rm -f netfileserver.o libnetfiles.o client.o