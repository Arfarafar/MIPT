CC = gcc
CFLAGS = -MD -g

all: a.out

a.out: avl.o diigraph.o tree.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $^

clean: 
	rm -rf a.out *.o *.d

-include *.d
