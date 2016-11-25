#Variable definition
CC  = gcc 
CCFLAGS = -Wall -g

#rules definition
all : compressR compressR_worker compressT

compressR : compressR_LOLS.c
	$(CC) $(CCFLAGS) -o compressR_LOLS compressR_LOLS.c

compressT : compressT_LOLS.c
	$(CC) $(CCFLAGS) -pthread -o compressT_LOLS compressT_LOLS.c
	
compressR_worker: compressR_worker_LOLS.c
	$(CC) $(CCFLAGS) -o compressR_worker_LOLS compressR_worker_LOLS.c

#This rule cleans up executable file
clean: 
	rm -f compressR_LOLS.o compressR_worker_LOLS.o compressT_LOLS.o
	
remove:

	rm -f *txt_LOLS*