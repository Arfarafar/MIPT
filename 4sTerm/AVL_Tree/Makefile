CC = gcc
CFLAGS = -MD -g --coverage

all: a.out 

a.out: avl.o diigraph.o tree.o
	$(CC) $^  $(CFLAGS) -o $@ 

%.o: %.c
	$(CC) $< -c  $(CFLAGS) -o $@ 

clean: 
	rm -rf a.out *.o *.d *.dot *.gcno *.gcda *.info

coverage: test.info
	genhtml -o report $<

test.info: st
	lcov -o test.info -c -d .

st: 
	./a.out

-include *.d
