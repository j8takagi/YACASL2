######################################################################
# テストグループのディレクトリー
# ほかに、Define.mkでも一部定義
######################################################################

# グループディレクトリー
GROUP_DIR := $(CURRDIR)

# グループ名。ディレクトリ名から取得
GROUP := $(notdir $(GROUP_DIR))

# テスト名。カレントディレクトリー内の、名前が大文字または.以外で始まるディレクトリー
TESTS = $(notdir $(shell $(FIND) -maxdepth 1 -name "[^A-Z.]*" -type d))

# テストグループログファイル
GROUP_LOG_FILE := $(shell $(ECHO) $(GROUP) | $(TR) '[a-z]' '[A-Z]').log

# テストグループレポートファイル
GROUP_REPORT_FILE := Report.log

# テストグループ実行時間ファイル
GROUP_TIME_FILE := $(shell echo $(GROUP) | $(TR) '[a-z]' '[A-Z]')_time.log

# グループで、テスト結果として作成されるファイル群
GROUP_RES_FILES := $(GROUP_LOG_FILE) $(GROUP_REPORT_FILE) $(GROUP_TIME_FILE)

# テストごとのログファイル
TEST_LOG_FILES := $(foreach test,$(TESTS),$(test)/$(LOG_FILE))

######################################################################
# テストグループのマクロ
######################################################################

# 指定したディレクトリーを作成
# 用例: $(call create_dir,name)
define create_dir
    $(call chk_var_null,$1)
    $(call chk_file_ext,$1)
    $(MKDIR) $1
endef

# リストで指定された親ディレクトリーにあるMakefileをインクルードするMakefileを作成
# 用例: $(call create_makefile,file,list_include_file)
define create_makefile
    $(RM) $1
    $(foreach infile,$2,$(ECHO) "include ../$(infile)" >>$1; )
endef

# テストごとのファイルをグループファイルに出力
# 引数は、テストのリスト、グループファイル、テストファイル
# 用例: $(call group_log,files_test_log,file_group_log)
define group_log
    $(foreach target,$1,$(call group_log_each,$(target),$2))
endef

# テストのログファイルをグループログファイルに出力。引数は、テスト、グループログファイル
# 用例: $(call group_log_each,file_test_log,file_group_log)
define group_log_each
    $(ECHO) $(dir $1) >>$2;
    if test -s $1; then $(CAT) $1 >>$2; else $(ECHO) $(dir $1)": no log" >>$2; fi
    $(ECHO) >>$2;

endef

# 成功したテストの数。テストグループログファイルから取得
SUCCESS_TEST = $(shell $(GREP) "^[^A-Z.].*: Test Success" $(GROUP_LOG_FILE) | $(LINECOUNT))

# 失敗したテストの数。テストグループログファイルから取得
FAIL_TEST = $(shell $(GREP) "^[^A-Z.].*: Test Failure" $(GROUP_LOG_FILE) | $(LINECOUNT))

# すべてのテストの数
ALL_TEST = $(shell $(EXPR) $(SUCCESS_TEST) + $(FAIL_TEST))

# テストごとの実行時間ファイル
TEST_TIME_FILES := $(foreach test,$(TESTS),$(test)/$(TIME_FILE))

# テストの結果を、グループログファイルを元にレポート。
# 引数は、グループ名、グループログファイル、グループレポートファイル
# 用例: $(call group_report,name,file_log,file_report)
define group_report
    $(ECHO) "$1: $(SUCCESS_TEST) / $(ALL_TEST) tests passed. Detail in $(GROUP_DIR)/$2" >$3;
    if test $(FAIL_TEST) -eq 0; then $(ECHO) "$1: All tests are succeded." >>$3; fi
endef

# リストで指定したディレクトリーでmakeを実行
# 用例: $(call make_tests,list_dir,target)
define make_tests
    $(foreach dir,$1,$(call make_test_each,$(dir),$2))
endef

# 指定したディレクトリーでmakeを実行
# 用例: $(call make_test_each,tests,target)
define make_test_each
    $(MAKE) $2 -sC $1;

endef
