CC ?= gcc
CFLAGS ?= -g -Wall -Wextra -I $(INCLUDE)
RMF ?= rm -f
WHICH ?= which
CTAGS ?= ctags
ETAGS ?= etags

INCLUDE := ../include

COMMONSRC := word.c cmem.c cerr.c
CASL2SRC := struct.c hash.c
ASSRC := assemble.c token.c label.c
EXECSRC := exec.c dump.c

.PHONY: all clean

all: ../casl2 ../comet2 ../dumpword ctags etags

../casl2: casl2.o $(subst .c,.o,$(COMMONSRC) $(CASL2SRC) $(ASSRC) $(EXECSRC))
	$(CC) $(CFLAGS) -o $@ $^
	@$(RMF) *.d

../comet2: comet2.o $(subst .c,.o,$(COMMONSRC) $(CASL2SRC) $(EXECSRC))
	$(CC) $(CFLAGS) -o $@ $^
	@$(RMF) *.d

../dumpword: dumpword.o $(subst .c,.o,$(COMMONSRC))
	$(CC) $(CFLAGS) -o $@ $^
	@$(RMF) *.d

%.o: %.c
	$(CC) -c $(CFLAGS) $<

%.d: %.c
	@$(CC) -MM -I $(INCLUDE) $< | sed 's/\($*\.o\):/\1 $@:/g' >$@

-include $(subst .c,.d,casl2.c comet2.c dumpword.c $(COMMONSRC) $(CASL2SRC) $(ASSRC) $(EXECSRC))

ctags: $(INCLUDE)/*.h *.c
	@($(WHICH) $(CTAGS) && $(CTAGS) $^) >/dev/null

etags: $(INCLUDE)/*.h *.c
	@($(WHICH) $(ETAGS) && $(ETAGS) $^) >/dev/null

clean: clean_cmd clean_build clean_tags

clean_cmd:
	@$(RMF) ../casl2 ../comet2 ../dumpword

clean_build:
	@$(RMF) *.o *.d

clean_tags:
	@$(RMF) TAGS tags
