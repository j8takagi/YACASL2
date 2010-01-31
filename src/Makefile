INCLUDE = ../include
CC = gcc
CFLAGS = -g -Wall -I $(INCLUDE)
COMMONSRC = word.o struct.o hash.o cmd.o cerr.o
ASSRC = assemble.o token.o label.o macro.o
EXECSRC = exec.o dump.o
.PHPNY: all clean
all: ../casl2 ../comet2 ../dumpword TAGS
../casl2: casl2.o $(COMMONSRC) $(ASSRC) $(EXECSRC)
	$(CC) $(CFLAGS) -o $@ $^
../comet2: comet2.o $(COMMONSRC) $(EXECSRC)
	$(CC) $(CFLAGS) -o $@ $^
../dumpword: dumpword.o $(COMMONSRC)
	$(CC) $(CFLAGS) -o $@ $^
%.o: %.c
	$(CC) -c $(CFLAGS) $<
casl2.o comet2.o $(COMMONSRC) $(ASSRC) $(EXECSRC): $(INCLUDE)/casl2.h $(INCLUDE)/word.h $(INCLUDE)/hash.h $(INCLUDE)/cerr.h
casl2.o $(ASSRC): $(INCLUDE)/assemble.h
comet2.o $(EXECSRC): $(INCLUDE)/exec.h
TAGS: $(INCLUDE)/*.h *.c
	@if test `which etags`; then etags $^; fi
clean:
	@rm -f *.o
