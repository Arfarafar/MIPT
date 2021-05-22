CC = g++
CFLAGS = -MD -g -pthread -lm -Wall

all: a.out 

a.out: find.o 
	$(CC) $^  $(CFLAGS) -o $@ 

%.o: %.cpp
	$(CC) $< -c  $(CFLAGS) -o $@ 

clean: 
	rm -rf a.out *.o *.d 


-include *.d


.PHONY: clean all 