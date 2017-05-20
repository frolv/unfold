PROGRAM_NAME := unfold
PREFIX ?= /usr/local

CC := gcc
CFLAGS := -Wall -Wextra -O2

RM := rm -f

.PHONY: all install clean

all: $(PROGRAM_NAME)

$(PROGRAM_NAME): unfold.c
	$(CC) -o $@ $< $(CFLAGS)

install: $(PROGRAM_NAME)
	install -d -m 0755 $(PREFIX)/bin
	install -m 0755 $< $(PREFIX)/bin

uninstall:
	$(RM) $(PREFIX)/bin/$(PROGRAM_NAME)

clean:
	$(RM) $(PROGRAM_NAME)
