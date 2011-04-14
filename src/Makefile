INCLUDE := ../include
CC := gcc
CFLAGS := -g -Wall -Wextra -I $(INCLUDE)
RM := rm -f

COMMONSRC := word.c cmem.c cerr.c
CASL2SRC := struct.c cmd.c hash.c
ASSRC := assemble.c token.c label.c
EXECSRC := exec.c dump.c

.PHONY: all clean

all: ../casl2 ../comet2 ../dumpword

../casl2: casl2.o $(subst .c,.o,$(COMMONSRC) $(CASL2SRC) $(ASSRC) $(EXECSRC))
	$(CC) $(CFLAGS) -o $@ $^

../comet2: comet2.o $(subst .c,.o,$(COMMONSRC) $(CASL2SRC) $(EXECSRC))
	$(CC) $(CFLAGS) -o $@ $^

../dumpword: dumpword.o $(subst .c,.o,$(COMMONSRC))
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

%.d: %.c
	@$(CC) -MM -I $(INCLUDE) $< | sed 's/\($*\.o\):/\1 $@:/g' >$@

-include $(subst .c,.d,casl2.c comet2.c dumpword.c $(COMMONSRC) $(CASL2SRC) $(ASSRC) $(EXECSRC))

TAGS: $(INCLUDE)/*.h *.c
	@if test `which etags`; then etags $^; fi

clean:
	@rm -f ../casl2 ../comet2 ../dumpword *.o *.d
