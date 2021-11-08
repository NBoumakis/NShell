# Development: Nikos Boumakis, 4346
# Email: csd4346@csd.uoc.gr


CC = gcc
CFLAGS = -Wall -std=gnu11 -pedantic -g

all: shell.out

shell.out: main.o parser.o parserUtilities.o shell.o
	$(CC) $(CFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f *.out ./*.o
