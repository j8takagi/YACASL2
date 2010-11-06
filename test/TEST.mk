# テストのテンプレート
# 次の変数を定義する
#    CMD: 実行するコマンド
#    MAINTARGET_OVERRIDE: メインターゲットを独自に定義し、オーバーライドする
# make         : CMDで設定されたコマンドを実行した出力結果を1.txtに出力し、0.txtと比較し、レポート
# make check   : ↓
# make prepare : CMDで設定されたコマンドを実行した出力結果を0.txt（テストの想定結果）に出力
# make clean   : 「make」で生成されたファイルをクリア
# make cleanall: 「make」と「make prepare」で生成されたファイルをクリア
UNITNAME = `pwd | xargs basename`
DATE = `date +"%F %T"`
ERR_FILE = err.txt
DIFF_FILE = diff.txt
REPORT_FILE = report.txt
TEST0_FILE = 0.txt
TEST1_FILE = 1.txt
.PHPNY: check prepare clean cleanall
check: clean $(REPORT_FILE)
prepare: cleanall $(TEST0_FILE)
clean:
	@rm -f $(TEST1_FILE) $(DIFF_FILE) $(REPORT_FILE) $(ERR_FILE)
cleanall: clean
	@rm -f $(TEST0_FILE)
ifndef MAINTARGET_OVERRIDE
$(TEST0_FILE) $(TEST1_FILE):
	@echo $(CMD) >$@; \
     $(CMD) >>$@ 2>$(ERR_FILE); \
     if test -s $(ERR_FILE); then cat $(ERR_FILE) >>$@; else rm -f $(ERR_FILE); fi
endif
$(DIFF_FILE): $(TEST1_FILE)
	@-diff -c $(TEST0_FILE) $(TEST1_FILE) >$@ 2>&1
$(REPORT_FILE): $(DIFF_FILE)
	@if test ! -s $^; then echo "$(UNITNAME): Test Success $(DATE)" >>$@; rm -f $^; else echo "$(UNITNAME): Test Failure $(DATE)" >>$@; fi;
