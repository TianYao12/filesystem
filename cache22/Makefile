flags=-O2 -Wall -std=c2x

all: cache22

cache22: cache22.o
	cc ${flags} $^ -o $@ ${ldflags}

cache22.o: cache22.c
	cc ${flags} -c $^

clean: 
	rm -f *.o cache22