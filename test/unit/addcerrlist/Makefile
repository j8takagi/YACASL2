include ../Define.mk
include ../Define_test.mk
include ../Test.mk

SRCDIR = ../../../src
SRCFILES = $(SRCDIR)/cmem.c $(SRCDIR)/cerr.c cmd.c

.INTERMEDIATE: cmd

cmd: cmd.c $(SRCFILES)
	gcc -g -Wall -I ../../../include -o $@ $^
	rm -rf cmd.dSYM
