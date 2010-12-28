include ../Define.mk
include ../Define_test.mk
include ../Test.mk

.INTERMEDIATE: cmd

SRCDIR = ../../../src
SRCFILES = $(SRCDIR)/cmem.c $(SRCDIR)/cerr.c $(SRCDIR)/cmd.c $(SRCDIR)/word.c $(SRCDIR)/struct.c $(SRCDIR)/token.c $(SRCDIR)/hash.c

cmd: cmd.c $(SRCFILES)
	gcc -g -Wall -I ../../../include -o $@ $^
	@rm -rf cmd.dSYM
