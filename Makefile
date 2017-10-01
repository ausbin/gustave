CC=gcc
CFLAGS=-std=gnu99 -pedantic -Wall -Werror
LIBS=-lgit2
BINARY=gustave
DEPS=$(wildcard *.h)
SOURCE=$(wildcard *.c)
OBJ=$(patsubst %.c,%.o,$(SOURCE))

.PHONY: install clean

$(BINARY): $(OBJ)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $<

install:
	install -Dm755 $(BINARY) /usr/local/bin/

clean:
	rm -f $(OBJ) $(BINARY)
