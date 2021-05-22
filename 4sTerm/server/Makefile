CC = gcc
CFLAGS = -MD -g -pthread -lm -std=c99

all: server.out client.out 

server.out: server.o 
	$(CC) $^  $(CFLAGS) -o $@ 

client: client.out 

client.out: client.o 
	$(CC) $^  $(CFLAGS) -o $@ 

%.o: %.c
	$(CC) $< -c  $(CFLAGS) -o $@ 


clean: 
	rm -rf *.out *.o *.d 



-include *.d


.PHONY: clean all 