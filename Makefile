PROGRAM_NAME := unfold
PREFIX ?= /usr/local

CC := gcc
CFLAGS := -Wall -Wextra -O2

RM := rm -f

MANFILE := $(PROGRAM_NAME).1

.PHONY: all install uninstall clean

all: $(PROGRAM_NAME)

$(PROGRAM_NAME): unfold.c
	$(CC) -o $@ $< $(CFLAGS)

install: $(PROGRAM_NAME)
	install -d -m 0755 $(PREFIX)/bin
	install -m 0755 $< $(PREFIX)/bin
	install -d -m 0755 $(PREFIX)/man/man1
	install -m 0644 $(MANFILE) $(PREFIX)/man/man1
	gzip -f $(PREFIX)/man/man1/$(MANFILE)
	mandb -f $(PREFIX)/man/man1/$(MANFILE)

uninstall:
	$(RM) $(PREFIX)/bin/$(PROGRAM_NAME)
	$(RM) $(PREFIX)/man/man1/$(MANFILE).gz
	mandb $(PREFIX)/man/man1

clean:
	$(RM) $(PROGRAM_NAME)
