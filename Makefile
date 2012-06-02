
CC ?= cc
INSTALL ?= install
RM ?= rm
LN ?= ln
MAKE ?= make
MD ?= mkdir
CP ?= cp
TAR ?= tar

D ?= /
PREFIX ?= /
BIN_PREFIX ?= $(PREFIX)/bin
CFLAGS ?= -Os

VERSION = $(shell cat VERSION)

all: cmdline

cmdline: cmdline.c
	$(CC) $(CFLAGS) -o $@ -Dcmdline_main=main $<

install:
	$(INSTALL) -D -m 0755 cmdline $(D)/$(BIN_PREFIX)
	$(INSTALL) -D -m 0744 isirc   $(D)/$(BIN_PREFIX)
	$(LN) -s bin/isirc $(D)/init
	$(LN) -s bin/isirc $(D)/linuxrc

clean:
	$(RM) cmdline target

tarball:
	ls isirc Makefile cmdline.c | pax -s "!^!isirc-$(VERSION)/!" -w | xz > isirc-$(VERSION).tar.xz

.PHONY: all install clean tarball
