# autotest.mk > test_template > Test.mk
# 自動テスト用のMakefile
#
# 要: Define.mk Define_test.mk
#
# オペレーター
# make         : CMDの標準出力をTEST1_FILEに保存したあと、TEST0_FILEとの差分を比較し、結果をLOG_FILEに出力
# make check   : ↓
# make set     : CMDの標準出力をTEST0_FILEに保存。TEST0_FILEが存在する場合は実行しない
# make reset   : CMDの標準出力をTEST0_FILEに保存。TEST0_FILEが存在する場合は上書き
# make time    : CMDの実行にかかった時間をTIME_FILEに保存し、出力
# make cleantime: "make time" で作成されたファイルをクリア
# make clean   : "make" で作成されたファイルをクリア
# make cleanall: "make" と "make set" で作成されたファイルをクリア

SHELL = /bin/bash

# テスト名。カレントディレクトリー名から取得
TEST = $(notdir $(CURRDIR))

# コマンドファイルのソース
CMDSRC_FILE := $(CMD_FILE)
#CMDSRC_FILE := $(CMD_FILE).c

.PHONY: check set reset time cleantime clean cleanall

check: clean $(DETAIL_FILE)
	@$(call disp_test_log,$(LOG_FILE))

checkall: clean $(DETAIL_FILE) $(TIME_FILE)
	@$(CAT) $(TIME_FILE) >>$(LOG_FILE)
	@$(call disp_test_log,$(LOG_FILE))

set:
	@-$(call exec_cmd,$(CMD_FILE),$(TEST0_FILE),$(ERR_FILE))
	@$(CAT) $(TEST0_FILE)

reset: cleanall set

time: cleantime $(TIME_FILE)

cleantime:
	@$(RM) $(TIME_FILE)

clean:
	@$(RM) $(TEST_RES_FILES)

cleanall: clean
	@$(RM) $(TEST0_FILE)

$(TEST1_FILE): $(CMD_FILE)
	@-$(call exec_cmd,$^,$@,$(ERR_FILE))

$(DIFF_FILE): $(TEST0_FILE) $(TEST1_FILE)
	@-$(call diff_files,$^,$@)

$(LOG_FILE): $(DIFF_FILE)
	@$(call test_log,$(TEST),$^,$@)

$(DETAIL_FILE): $(LOG_FILE)
	@$(call report_files,$(LOG_FILE) $(CMDSRC_FILE) $(TEST0_FILE) $(ERR_FILE) $(DIFF_FILE) $(TEST1_FILE),$@)

$(TIME_FILE): $(CMD_FILE)
	$(call time_cmd,$^,$@)
