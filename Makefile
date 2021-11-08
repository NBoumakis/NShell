
CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

all: shell.out

shell.out: main.o parser.o parserUtilities.o shell.o
	$(CC) $(CFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f *.out ./*.o
