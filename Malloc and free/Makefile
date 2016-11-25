#Variable definition
CC  = gcc 
CCFLAGS = -Wall -g

#rules definition
all : memgrind

memgrind : memgrind.c mymalloc.h
	$(CC) $(CCFLAGS) -o memgrind memgrind.c mymalloc.c

#This rule cleans up executable file
clean: 
	rm -f mymalloc.o memgrind.o