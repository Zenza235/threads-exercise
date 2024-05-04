CC = gcc
CFLAGS = -ansi -Wall -g -O0 -Wwrite-strings -Wshadow -pedantic-errors \
-fstack-protector-all

.PHONY: all clean

PROGS = threads

all: $(PROGS)

clean:
	rm -f *.o $(PROGS) a.out

threads: threads.o
	$(CC) -o threads -lpthread threads.o

threads.o: threads.c
	$(CC) $(CFLAGS) -c threads.c