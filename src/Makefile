INCLUDE = ../include
CC = gcc
CFLAGS = -g -Wall -I $(INCLUDE)
WORDOBJ = word.o
HASHOBJ = hash.o
CMEMOBJ = cmem.o
CERROBJ = cerr.o
COMMONOBJ = $(WORDOBJ) $(HASHOBJ) $(CMEMOBJ) $(CERROBJ) struct.o cmd.o
ASOBJ = assemble.o token.o label.o macro.o
EXECOBJ = exec.o dump.o
COMMONHEAD = $(INCLUDE)/casl2.h $(INCLUDE)/word.h $(INCLUDE)/hash.h $(INCLUDE)/cmem.h $(INCLUDE)/cerr.h
ASHEAD = $(INCLUDE)/assemble.h
EXECHEAD = $(INCLUDE)/exec.h

.PHONY: all clean

all: ../casl2 ../comet2 ../dumpword

../casl2: casl2.o $(COMMONOBJ) $(ASOBJ) $(EXECOBJ)
	$(CC) $(CFLAGS) -o $@ $^

../comet2: comet2.o $(COMMONOBJ) $(EXECOBJ)
	$(CC) $(CFLAGS) -o $@ $^

../dumpword: dumpword.o $(WORDOBJ) $(CMEMOBJ) $(CERROBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

casl2.o comet2.o $(COMMONOBJ) $(ASOBJ) $(EXECOBJ): $(COMMONHEAD)

casl2.o $(ASOBJ): $(ASHEAD)

comet2.o $(EXECOBJ): $(EXECHEAD)

TAGS: $(INCLUDE)/*.h *.c
	@if test `which etags`; then etags $^; fi

clean:
	@rm -f *.o
