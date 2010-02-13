# テストのテンプレート
# make         : CMDで設定されたコマンドを実行した出力結果を1.txtに出力し、0.txtと比較し、レポート
# make check   : ↓
# make prepare : CMDで設定されたコマンドを実行した出力結果を0.txt（テストの想定結果）に出力
# make clean   : 「make」で生成されたファイルをクリア
# make cleanall: 「make」と「make clean」で生成されたファイルをクリア
ERRFILE = err.txt
UNITNAME = `pwd | xargs basename`

.PHPNY: check prepare clean cleanall
check: clean report.txt
prepare: cleanall 0.txt $(OBJFILE)
clean:
	@rm -f 1.txt diff.txt report.txt err.txt
cleanall: clean
	@rm -f 0.txt $(OBJFILE)
$(OBJFILE): $(CASL2) $(ASFILE)
	@-$(ASCMD)
0.txt 1.txt: $(COMET2) $(OBJFILE)
	@echo $(EXECCMD) >$@; \
     if test -s $(OBJFILE); then $(EXECCMD) >>$@ 2>$(ERRFILE); fi; \
     if test -s $(ERRFILE); then cat err.txt >>$@; else rm -f $(ERRFILE); fi
diff.txt: 1.txt
	@-diff -c 0.txt 1.txt >$@ 2>&1
report.txt: diff.txt
	@echo -n "$(UNITNAME): Test " >$@; \
     if test ! -s $^; then echo -n "Success " >>$@; rm -f $^; else echo -n "Failure " >>$@; fi; \
     echo `date +"%F %T"` >>$@
