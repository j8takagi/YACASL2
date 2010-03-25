# テストのテンプレート
# make         : CMDで設定されたコマンドを実行した出力結果を1.txtに出力し、0.txtと比較し、レポート
# make check   : ↓
# make prepare : CMDで設定されたコマンドを実行した出力結果を0.txt（テストの想定結果）に出力
# make clean   : 「make」で生成されたファイルをクリア
# make cleanall: 「make」と「make clean」で生成されたファイルをクリア
UNITNAME = `pwd | xargs basename`

.PHPNY: check prepare clean cleanall
check: clean report.txt
prepare: cleanall 0.txt
clean:
	@rm -f 1.txt diff.txt report.txt err.txt
cleanall: clean
	@rm -f 0.txt
ifndef SETBYOTHER
0.txt 1.txt: $(CASL2) $(ASFILE)
	@echo $(CMD) >$@; \
     $(CMD) >>$@ 2>err.txt; \
     if test -s err.txt; then cat err.txt >>$@; else rm -f err.txt; fi
endif
diff.txt: 1.txt
	@-diff -c 0.txt 1.txt >$@ 2>&1
report.txt: diff.txt
	@echo -n "$(UNITNAME): Test " >$@; \
     if test ! -s $^; then echo -n "Success " >>$@; rm -f $^; else echo -n "Failure " >>$@; fi; \
     echo `date +"%F %T"` >>$@
