# autotest.mk > test_template > Define.mk
# 自動テスト用の変数、マクロ定義

ifndef DEFINE_INCLUDED
DEFINE_INCLUDED = 1

# 現在の日時
DATE = $(shell date +"%F %T")

# Makefile
MAKEFILE := Makefile

######################################################################
# コマンド
######################################################################

CAT := cat
CHMOD := chmod
CP := cp
DEV_NULL := /dev/null
DIFF := diff -c
ECHO := echo
EXPR := expr
FIND := find
GREP := grep
LINECOUNT := wc -l
LN := ln -s
MKDIR := mkdir -p
MV := mv
SED := sed
TIME := time
TR := tr
VALGRIND := valgrind
VALGRINDFLAG :=  -q --leak-check=full --log-file=valgrind.log

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
    $(if $(wildcard $1),$(error $1 exists in $(CURDIR)))
endef

# 指定したディレクトリーを作成
# 引数は、ディレクトリー名
# 用例: $(call create_dir,name)
define create_dir
    $(call chk_var_null,$1)
    $(call chk_file_ext,$1)
    $(MKDIR) $1
endef

# テストディレクトリーのMakefileを作成
# 引数は、Makefile名、依存ファイル群
# 用例: $(call create_makefile,file,list_include_file)
define create_makefile
    $(RM) $1
    $(foreach infile,$2,$(ECHO) "include ../$(infile)" >>$1; )
    $(if $(filter $(SRC),c),$(call puts_cmd_c,$1))
endef

# C言語の関数をテストするための設定を、指定されたファイルに出力
# 引数は、ファイル名
# 用例: $(call puts_cmd_c,file)
define puts_cmd_c
    $(ECHO) >>$1
    $(ECHO) "CC := gcc" >>$1
    $(ECHO) "CFLAGS := -Wall" >>$1
    $(ECHO) >>$1
    $(ECHO) ".INTERMEDIATE:" "$$""(CMD_FILE)" >>$1
    $(ECHO) >>$1
    $(ECHO) "CMDSRC_FILE := cmd.c" >>$1
    $(ECHO) "TESTTARGET_FILES :=       # Set test target files" >>$1
    $(ECHO) >>$1
    $(ECHO) "$$""(CMD_FILE):" "$$""(CMDSRC_FILE)" "$$""(TESTTARGET_FILES)" >>$1
    $(ECHO) "	""$$""(CC)" "$$""(CFLAGS)" "-o" "$$""@" "$$""^" >>$1
endef

######################################################################
# テストグループのディレクトリー
######################################################################

# テストグループとテストの変数を定義したMakefile
DEFINE_FILE := Define.mk

# テストのターゲットを定義したMakefile
TEST_MAKEFILE := Test.mk

# すべてのMakefile群
MAKEFILES := $(DEFINE_FILE) $(TEST_MAKEFILE)

# すべてのMakefile群の絶対パス
MAKEFILES_ABS := $(foreach file,$(MAKEFILES),$(CURDIR)/$(file))

######################################################################
# テストのディレクトリー
######################################################################

# テストコマンドファイル
CMD_FILE := cmd

# テスト説明ファイル
DESC_FILE := desc.txt

# テスト想定ファイル
TEST0_FILE := 0.txt

# テスト結果ファイル
TEST1_FILE := 1.txt

# テストの、想定結果と結果の差分ファイル
DIFF_FILE := diff.log

# テストエラーファイル
ERR_FILE := err.log

# テストログファイル
LOG_FILE := summary.log

# 実行時間ファイル
TIME_FILE := time.log

# valgrindによるメモリーチェックファイル
VALGRIND_FILE := valgrind.log

# テスト詳細レポートファイル
DETAIL_FILE := detail.log

# テストの結果として作成されるファイル群
TEST_RES_FILES := $(TEST1_FILE) $(DIFF_FILE) $(ERR_FILE) $(LOG_FILE) $(DETAIL_FILE) $(TIME_FILE)

endif
