# テストのテンプレート
# make         : CMDで設定されたコマンドを実行した出力結果を1.txtに出力し、0.txtと比較し、レポート
# make check   : ↓
# make prepare : CMDで設定されたコマンドを実行した出力結果を0.txt（テストの想定結果）に出力
# make clean   : 「make」で生成されたファイルをクリア
# make cleanall: 「make」と「make report」で生成されたファイルをクリア
LOGFILE = ../TEST.log
ERRFILE = err.txt
TESTSH = test.sh

.PHPNY: check prepare clean cleanall
check: clean report.txt
prepare: cleanall 0.txt
clean:
	@rm -f 1.txt diff.txt report.txt
cleanall: clean
	@rm -f 0.txt
0.txt 1.txt: $(DUMPWORD) $(TESTSH)
	@sh $(TESTSH) >>$@ 2>$(ERRFILE); \
     if test -s $(ERRFILE); then cat err.txt >>$@; else rm -f $(ERRFILE); fi
$(TESTSH): ../HEX.txt
	@xargs -n1 echo "$(DUMPWORD) $(DUMPWORDFLAG) " <$^ >$@
diff.txt: 1.txt
	@-diff 0.txt 1.txt >$@ 2>&1
report.txt: diff.txt
	@echo -n "$(UNITNAME): Test " >$@; \
     if test ! -s $^; then echo -n "Success " >>$@; rm -f $^; else echo -n "Failure " >>$@; fi; \
     echo `date +"%F %T"` >>$@