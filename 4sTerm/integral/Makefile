CC = gcc
CFLAGS = -MD -g -pthread -lm -std=c99

all: a.out 

a.out: integral.o 
	$(CC) $^  $(CFLAGS) -o $@ 

%.o: %.c
	$(CC) $< -c  $(CFLAGS) -o $@ 

clean: 
	rm -rf a.out *.o *.d 


-include *.d


.PHONY: clean all 