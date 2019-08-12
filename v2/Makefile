CC=gcc
CFLAGS=-g -Wall

src = $(wildcard src/*.c)

obj = $(src:.c=.o)

.PHONY: all
all: main

.PHONY: clean
clean:
	rm -f $(obj)

main: $(obj)
	$(CC) -o $@ $^