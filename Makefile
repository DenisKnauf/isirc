
CC ?= cc
INSTALL ?= install
RM ?= rm

D ?= /
PREFIX ?= /
BIN_PREFIX ?= $(PREFIX)/bin
CFLAGS ?= -Os

all: cmdline

cmdline: cmdline.c
	$(CC) $(CFLAGS) -o $@ -Dcmdline_main=main $<

install:
	$(INSTALL) -D -m 0755 cmdline $(D)/$(BIN_PREFIX)
	$(INSTALL) -D -m 0744 isirc   $(D)/$(BIN_PREFIX)
	$(LN) -s isirc $(D)/init
	$(LN) -s isirc $(D)/linuxrc

clean:
	$(RM) cmdline

.PHONY: all install clean
