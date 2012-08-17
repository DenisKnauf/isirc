
CC ?= cc
INSTALL ?= install
RM ?= rm
LN ?= ln
MAKE ?= make
MD ?= mkdir
MD_P ?= $(MD) -p
CP ?= cp
TAR ?= tar

D ?= /
PREFIX ?= /
BIN_PREFIX ?= $(PREFIX)/bin
CFLAGS ?= -Os

VERSION = $(shell cat VERSION)

help:
	@echo "Choose:"
	@echo "make all - build all files"
	@echo "make install - install isirc"
	@echo "make tarball - build source-tarball"
	@echo "make clean - removes build-files"

all: cmdline

cmdline: cmdline.c
	$(CC) $(CFLAGS) -o $@ -Dcmdline_main=main $<

install:
	$(MD_P) $(D)/$(BIN_PREFIX)
	$(INSTALL) -D -m 0755 cmdline $(D)/$(BIN_PREFIX)
	$(INSTALL) -D -m 0744 isirc   $(D)/$(BIN_PREFIX)
	$(LN) -s bin/isirc $(D)/init
	$(LN) -s bin/isirc $(D)/linuxrc

clean:
	$(RM) cmdline isirc-*.tar.xz

tarball:
	ls isirc Makefile cmdline.c | pax -s "!^!isirc-$(VERSION)/!" -w | xz > isirc-$(VERSION).tar.xz

.PHONY: all install clean tarball
