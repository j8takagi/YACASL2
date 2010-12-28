include ../Define.mk
include ../Define_test.mk
include ../Test.mk

.INTERMEDIATE: cmd

SRCDIR = ../../../src
SRCFILES = $(SRCDIR)/hash.c $(SRCDIR)/cmem.c $(SRCDIR)/cerr.c $(SRCDIR)/cmd.c $(SRCDIR)/assemble.c $(SRCDIR)/word.c $(SRCDIR)/label.c $(SRCDIR)/struct.c $(SRCDIR)/token.c

cmd: cmd.c $(SRCFILES)
	gcc -g -Wall -I ../../../include -o $@ $^
