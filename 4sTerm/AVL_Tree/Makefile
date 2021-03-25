CC = gcc
CFLAGS = -MD -g --coverage

all: a.out 

a.out: test.o tree.o
	$(CC) $^  $(CFLAGS) -o $@ 

%.o: %.c
	$(CC) $< -c  $(CFLAGS) -o $@ 

clean: 
	rm -rf a.out *.o *.d *.dot *.gcno *.gcda *.info report

coverage: test.info
	genhtml -o report $<
	google-chrome report/index.html

test.info: st
	lcov -o test.info -c -d .

st: 
	./a.out

-include *.d


.PHONY: clean all coverage