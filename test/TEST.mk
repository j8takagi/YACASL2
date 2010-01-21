# テストのテンプレート
# make         : CMDで設定されたコマンドを実行した出力結果を1.txtに出力し、0.txtと比較し、レポート
# make all     : ↓
# make check   : ↓
# make prepare : CMDで設定されたコマンドを実行した出力結果を0.txt（テストの想定結果）に出力
# make clean   : 「make」で生成されたファイルをクリア
# make cleanall: 「make」と「make clean」で生成されたファイルをクリア
LOGFILE = ../test.log
ERRFILE = err.txt
CURDIR = `pwd`
define report
  @echo -n "`basename $$PWD`: Test " >$@
  @if test ! -s $^; then \
    echo -n "Success " >>$@; \
    rm -f $^; \
  else \
    echo -n "Failure " >>$@; \
  fi
  @echo `date +"%F %T"` >>$@
endef

.PHPNY: all check prepare clean cleanall logadd
check: clean logadd
all: check
prepare: cleanall 0.txt
clean:
	@rm -f 1.txt diff.txt report.txt
cleanall: clean
	@rm -f 0.txt
0.txt 1.txt: $(CMDFILE) $(OBJFILE)
	@$(CMD) >$@ 2>$(ERRFILE); \
     if test -s $(ERRFILE); then cat err.txt >>$@; else rm -f $(ERRFILE); fi
diff.txt: 1.txt
	@-diff 0.txt 1.txt >$@ 2>&1
report.txt: diff.txt
	$(report)
logadd: report.txt
	@cat $^ >>$(LOGFILE)
