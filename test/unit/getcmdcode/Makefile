include ../Define.mk
include ../Define_test.mk
include ../Test.mk

.INTERMEDIATE: cmd

SRCDIR = ../../../src
SRCFILES = $(SRCDIR)/word.c $(SRCDIR)/struct.c $(SRCDIR)/hash.c $(SRCDIR)/cmd.c $(SRCDIR)/cmem.c  $(SRCDIR)/cerr.c

cmd: cmd.c $(SRCFILES)
	gcc -g -Wall -I ../../../include -o $@ $^
