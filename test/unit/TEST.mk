# テストのテンプレート
# make         : CMDで設定されたコマンドを実行した出力結果を1.txtに出力し、0.txtと比較し、レポート
# make check   : ↓
# make prepare : CMDで設定されたコマンドを実行した出力結果を0.txt（テストの想定結果）に出力
# make clean   : 「make」で生成されたファイルをクリア
# make cleanall: 「make」と「make clean」で生成されたファイルをクリア
ERRFILE = err.txt
UNITNAME = `pwd | xargs basename`

SRCDIR = ../../../src
INCLUDE = ../../../include
CC = gcc
CFLAGS = -g -Wall -I $(INCLUDE)
COMMONSRC = $(SRCDIR)/word.c $(SRCDIR)/struct.c $(SRCDIR)/hash.c $(SRCDIR)/cmd.c $(SRCDIR)/cerr.c
ASSRC = $(SRCDIR)/assemble.c $(SRCDIR)/token.c $(SRCDIR)/label.c $(SRCDIR)/macro.c
EXECSRC = $(SRCDIR)/exec.c $(SRCDIR)/dump.c

ifeq "$(UCLASS)" "AS"
  SRC = $(COMMONSRC) $(ASSRC)
endif
ifeq "$(UCLASS)" "EXEC"
  SRC = $(COMMONSRC) $(EXECSRC)
endif
ifeq "$(UCLASS)" "COMMON"
  SRC = $(COMMONSRC)
endif

.PHPNY: check prepare clean cleanall
check: clean report.txt
prepare: cleanall 0.txt
clean:
	@rm -f a.out 1.txt diff.txt report.txt
cleanall: clean
	@rm -f 0.txt
a.out: $(SRC) $(TESTSRCFILE)
	@gcc $(CFLAGS) $(SRC) $(TESTSRCFILE)
0.txt 1.txt: a.out
	@./a.out >$@ 2>&1
diff.txt: 1.txt
	@-diff -c 0.txt 1.txt >$@ 2>&1
report.txt: diff.txt
	@echo -n "$(UNITNAME): Test " >$@; \
     if test ! -s $^; then echo -n "Success " >>$@; rm -f $^; else echo -n "Failure " >>$@; fi; \
     echo `date +"%F %T"` >>$@
