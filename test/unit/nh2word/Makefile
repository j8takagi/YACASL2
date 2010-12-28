include ../Define.mk
include ../Define_test.mk
include ../Test.mk

.INTERMEDIATE: cmd

SRCDIR = ../../../src
SRCFILES = cmd.c $(SRCDIR)/cmem.c $(SRCDIR)/cerr.c $(SRCDIR)/word.c

cmd: cmd.c $(SRCFILES)
	gcc -g -Wall -I ../../../include -o $@ $^
