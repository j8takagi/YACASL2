LOGFILE = ../test.log
INCLUDE = ../../../include
CC = gcc
CFLAGS = -g -Wall -I $(INCLUDE)
COMMONSRC = ../../../src/struct.c ../../../src/cmd.c ../../../src/cerr.c
ASSRC = ../../../src/assemble.c ../../../src/token.c ../../../src/label.c ../../../src/macro.c
EXECSRC = ../../../exec.c ../../../dump.c
ifeq "$(UCLASS)" "AS"
  SRC = $(COMMONSRC) $(ASSRC)
endif
ifeq "$(UCLASS)" "EXEC"
  SRC = $(COMMONSRC) $(EXECSRC)
endif
ifeq "$(UCLASS)" "COMMON"
  SRC = $(COMMONSRC)
endif
define report
  @echo -n "$(UNITNAME): Test " >$@
  @if test ! -s $^; then \
    echo -n "Success " >>$@; \
  else \
    echo -n "Failure " >>$@; \
  fi
  @echo `date +"%F %T"` >>$@
endef

.PHPNY: all prepare clean
all: check
prepare: orgclean testclean 0.txt
check: testclean logadd
orgclean:
	@rm -f 0.txt
testclean:
	@rm -f 1.txt diff.txt report.txt
$(UNITNAME): $(COMMONSRC) $(ASSRC) $(UNITNAME).c
	gcc $(CFLAGS) -o $(UNITNAME) $(SRC) $(UNITNAME).c
0.txt 1.txt: $(UNITNAME)
	./$(UNITNAME) >$@ 2>&1
diff.txt: 1.txt
	@-diff 0.txt 1.txt >$@ 2>&1
report.txt: diff.txt
	$(report)
logadd: report.txt
	@cat $^ >>$(LOGFILE)
