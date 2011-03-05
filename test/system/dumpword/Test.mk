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

######################################################################
# マクロ
######################################################################

# 指定されたファイルをチェックし、空の場合は削除
# 引数は、対象ファイル
# 用例: $(call rm_null,file_target)
define rm_null
    if test ! -s $1; then $(RM) $1; fi
endef

# 説明ファイルの内容を、指定されたファイルに出力
# 引数は、出力ファイル
# 用例: $(call desc_log,file_out)
define desc_log
    if test -s $(DESC_FILE); then $(CAT) $(DESC_FILE) >>$1; fi
endef

# テスト実行の経過時間を、指定されたファイルに出力して表示
# 引数は、テスト名、コマンドファイル、出力ファイル
# 用例: $(call time_cmd,file_cmd,file_out)
define time_cmd
    if test ! -x $1; then $(CHMOD) u+x $1; fi
    ($(TIME) ./$1 1>$(DEV_NULL) 2>$(DEV_NULL)) 2>&1 | $(GREP) '^real' >$2
endef

# テスト実行コマンド。
# コマンドファイルを実行し、標準出力を指定されたファイルに保存。
# エラー発生時は、エラー出力を出力ファイルとエラーファイルに保存。
# 引数は、コマンドファイル、出力ファイル、エラーファイル
# 用例: $(call exec_cmd,file_cmd,file_out,file_err)
define exec_cmd
    if test ! -x $1; then $(CHMOD) u+x $1; fi
    ./$1 >>$2 2>$3
    if test -s $3; then $(CAT) $3 >>$2; fi
    $(CP) $2 $2.tmp && $(SED) -e "s%$(CURRDIR)%\$$PWD%g" $2.tmp >$2 && $(RM) $2.tmp
    $(call rm_null,$3)
endef

# 2つのファイルを比較し、差分ファイルを作成
# 引数は、2ファイルのリスト、差分ファイル
# 用例: $(call diff_files,files,file_out)
define diff_files
    $(DIFF) $1 >$2 2>&1
    $(call rm_null,$2)
endef

# 差分ファイルの内容をログファイルに出力
# 引数は、テスト名、差分ファイル、ログファイル
# 用例: $(call test_log,name,file_diff,file_log)
define test_log
    $(call desc_log,$3)
    if test ! -s $2; then RES=Success; else RES=Failure; fi; $(ECHO) "$1: Test $$RES $(DATE)" >>$3
    $(ECHO) "Details in $(CURRDIR)/$(DETAIL_FILE)" >>$3
endef

# NODISPが設定されていない時は、ログファイルを表示
# 引数は、ログファイル
# 用例: $(call disp_test_log,file_log)
define disp_test_log
    $(if $(NODISP),,$(CAT) $1)
endef

# ファイル群から、ファイル名とファイルの内容を指定されたファイルに出力
# 引数は、対象ファイル群、出力ファイル
# 用例: $(call report_files,list_file_target,file_out)
define report_files
    $(foreach tfile,$1,$(call report_file,$(tfile),$2))
endef

# ファイル名とファイルの内容を指定されたファイルに出力
# 引数は、対象ファイル、出力ファイル
# 用例: $(call report_file,file_target,file_out)
define report_file
    $(call chk_var_null,$1)
    if test -s $1; then $(ECHO) "== $1 ==" >>$2; $(call echo_hr,$2); cat $1 >>$2; $(call echo_hr,$2); $(ECHO) >>$2; fi
endef

define echo_hr
    $(ECHO) "----------------------------------------------------------------------" >>$1
endef

# テスト名。カレントディレクトリー名から取得
TEST = $(notdir $(CURRDIR))

# コマンドファイルのソース
CMDSRC_FILE ?= $(CMD_FILE)

.PHONY: check set reset clean cleanall time cleantime

check: clean $(DETAIL_FILE)
	@$(call disp_test_log,$(LOG_FILE))

checkall: clean $(DETAIL_FILE) $(TIME_FILE)
	@$(CAT) $(TIME_FILE) >>$(LOG_FILE)
	@$(call disp_test_log,$(LOG_FILE))

set: $(CMD_FILE)
	@$(call chk_file_ext,$(TEST0_FILE))
	@-$(call exec_cmd,$(CMD_FILE),$(TEST0_FILE),$(ERR_FILE))
	@$(CAT) $(TEST0_FILE)

reset: cleanall $(CMD_FILE)
	@-$(call exec_cmd,$(CMD_FILE),$(TEST0_FILE),$(ERR_FILE))
	@$(CAT) $(TEST0_FILE)

clean:
	@$(RM) $(TEST_RES_FILES)

cleanall: clean
	@$(RM) $(TEST0_FILE)

time: cleantime $(TIME_FILE)
	@$(CAT) $(TIME_FILE)

cleantime:
	@$(RM) $(TIME_FILE)

$(TEST1_FILE): $(CMD_FILE)
	@-$(call exec_cmd,$^,$@,$(ERR_FILE))

$(DIFF_FILE): $(TEST0_FILE) $(TEST1_FILE)
	@-$(call diff_files,$^,$@)

$(LOG_FILE): $(DIFF_FILE)
	@$(call test_log,$(TEST),$^,$@)

$(DETAIL_FILE): $(LOG_FILE)
	@$(call report_files,$(LOG_FILE) $(CMDSRC_FILE) $(TEST0_FILE) $(ERR_FILE) $(DIFF_FILE) $(TEST1_FILE),$@)

$(TIME_FILE): $(CMD_FILE)
	@$(call time_cmd,$^,$@)
