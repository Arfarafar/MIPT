CC = gcc
CFLAGS = -pthread

all: a.out


a.out: 1.o
	$(CC) $(CFLAGS) -o $@ $^


1.o: matrix.c
	$(CC) -c $(CFLAGS) -o $@ $<


clean:
	rm -rf a.out *.o

