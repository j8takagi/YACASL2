######################################################################
# マクロ
######################################################################

# 引数のファイルをチェックし、内容がない場合は削除
# 用例: $(call rm_null,file)
define rm_null
    if test ! -s $1; then $(RM) $1; fi
endef

# 説明ファイルの内容を、引数のファイルに出力
# 用例: $(call desc_log,file_out)
define desc_log
    if test -s $(DESC_FILE); then $(CAT) $(DESC_FILE) >>$1; fi
endef

# テスト実行の経過時間を、ファイルに出力して表示
# 引数は、テスト名、コマンドファイル、出力ファイル
# 用例: $(call time_cmd,name,file_cmd,file_out)
define time_cmd
    $(call chk_file_notext,$2)
    $(CHMOD) u+x $2
    $(TIME) -f"$1: %E" -o $3 ./$2 >$(DEV_NULL) 2>&1
endef

# テスト実行コマンド。引数は、コマンドファイル、出力ファイル、エラーファイル
# コマンドファイルを実行し、標準出力を出力ファイルに保存。
# エラー発生時は、エラー出力を出力ファイルとエラーファイルに保存。
# 用例: $(call exec_cmd,file_cmd,file_out,file_err)
define exec_cmd
    $(call chk_file_notext,$1)
    $(CHMOD) u+x $1
    ./$1 >>$2 2>$3
    if test -s $3; then $(CAT) $3 >>$2; fi
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
    $(ECHO) "Detail in $(CURRDIR)/$(DETAIL_FILE)" >>$3
endef

# NODISPが設定されていない時は、ログファイルを表示
# 引数は、ログファイル
# 用例: $(call disp_test_log,file_log)
define disp_test_log
    $(if $(NODISP),,$(CAT) $1)
endef

# ファイル群から、ファイル名とファイルの内容を出力
# 引数は、対象ファイル群、出力ファイル
# 用例: $(call report_files,list_file_target,file_out)
define report_files
    $(call chk_file_ext,$2)
    $(foreach tfile,$1,$(call report_file,$(tfile),$2))
endef

# ファイル名とファイルの内容を出力
# 引数は、対象ファイル、出力ファイル
# 用例: $(call report_file,file_target,file_out)
define report_file
    $(call chk_var_null,$1)
    if test -s $1; then $(ECHO) "== $1 ==" >>$2; $(call echo_hr,$2); cat $1 >>$2; $(call echo_hr,$2); $(ECHO) >>$2; fi
endef

define echo_hr
    $(ECHO) "----------------------------------------------------------------------" >>$1
endef
