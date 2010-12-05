# autotest.mk > test_template > Define.mk
# 自動テスト用の変数、マクロ定義

ifndef DEFINE_INCLUDED
DEFINE_INCLUDED = 1

# 現在の日時
DATE = $(shell date +"%F %T")

# 現在のディレクトリー
CURRDIR = $(shell pwd)

# Makefile
MAKEFILE := Makefile

######################################################################
# コマンド
######################################################################

CP ?= cp

CAT ?= cat

MKDIR ?= mkdir

RM ?= rm -f

ECHO ?= echo

TIME ?= /usr/bin/time --quiet

DIFF ?= diff -c

DEV_NULL ?= /dev/null

CHMOD ?= chmod

GREP ?= grep

LINECOUNT ?= wc -l

FIND ?= find

TR ?= tr

EXPR ?= expr

LINK ?= ln -s

######################################################################
# テストグループとテストでの共通マクロ
######################################################################

# chk_var_null: 引数がNULLの場合、エラー
# 用例: $(call chk_var_null,var)
define chk_var_null
    $(if $1,,$(error NULL argument))
endef

# chk_file_ext: 指定されたファイルが実在する場合、エラー
# 用例: $(call chk_file_ext,file)
define chk_file_ext
    $(if $(wildcard $1),$(error $1 exists in $(CURRDIR)))
endef

# chk_file_notext: 指定されたファイルが実在しない場合、エラー
# 用例: $(call chk_file_notext,file)
define chk_file_notext
    $(if $(wildcard $1),,$(error $1 not exists in $(CURRDIR)))
endef

######################################################################
# テストグループのディレクトリー
######################################################################

# テストグループとテストの変数を定義したMakefile
DEF_FILE := Define.mk

# テストグループの変数を定義したMakefile
DEF_GROUP_FILE := Define_group.mk

# テストの変数を定義したMakefile
DEF_TEST_FILE := Define_test.mk

# テストのターゲットを定義したMakefile
TEST_MAKEFILE := Test.mk

# すべてのMakefile群
MAKEFILES := $(DEF_FILE) $(DEF_GROUP_FILE) $(DEF_TEST_FILE) $(TEST_MAKEFILE)

MAKEFILES_ABS := $(foreach file,$(MAKEFILES),$(CURRDIR)/$(file))

# テストごとのMakefileでインクルードするMakefile群
TEST_MAKEFILES := $(DEF_FILE) $(DEF_TEST_FILE) $(TEST_MAKEFILE)

######################################################################
# テストのディレクトリー
######################################################################

# テストコマンドファイル
CMD_FILE := cmd

# テスト説明ファイル
DESC_FILE := desc.txt

# テスト想定結果ファイル
TEST0_FILE := 0.txt

# テスト結果ファイル
TEST1_FILE := 1.txt

# テストの、想定結果と結果の差分ファイル
DIFF_FILE := diff.txt

# テストエラーファイル
ERR_FILE := err.txt

# テストログファイル
LOG_FILE := test.log

# 実行時間ファイル
TIME_FILE := time.log

# テスト詳細レポートファイル
DETAIL_FILE := detail.log

# テストの結果として作成されるファイル群
TEST_RES_FILES := $(TEST1_FILE) $(DIFF_FILE) $(ERR_FILE) $(LOG_FILE) $(DETAIL_FILE) $(TIME_FILE)

endif
