# autotest.mk > template > Group.mk
# テストグループのMakefile
#
# オペレーター
# make          : すべてのテストを実行し、結果をログファイルに出力
# make check    : ↓
# make checkall : すべてのテストを実行し、結果と実行時間をログファイルに出力
# make time     : すべてのテストを実行し、実行時間をログファイルに出力
# make create   : TESTNAMEで指定されたテストを新規に作成
# make clean    : すべてのテストで、"make" で生成されたファイルをクリア
# make cleantime: すべてのテストで、実行時間のログファイルをクリア

SHELL = /bin/sh

include Define.mk
include Define_group.mk

.PHONY: check checkall time create clean cleantime

check checkall: clean $(GROUP_REPORT_FILE)
	@$(CAT) $(GROUP_REPORT_FILE)

time: cleantime $(GROUP_TIME_FILE)
	@$(CAT) $(GROUP_TIME_FILE)

create:
	@$(call create_dir,$(TEST))
	@$(call create_makefile,$(TEST)/$(MAKEFILE),$(TEST_MAKEFILES))

clean:
	@$(call make_tests,$(TESTS),$@)
	@$(RM) $(GROUP_RES_FILES)

cleantime:
	@$(call make_tests,$(TESTS),$@)
	@$(RM) $(GROUP_TIME_FILE)

$(GROUP_REPORT_FILE): $(GROUP_LOG_FILE)
	@$(call group_report,$(GROUP),$^,$@)

$(GROUP_LOG_FILE): $(TEST_LOG_FILES)
	@$(call group_log,$^,$@)

$(TEST_LOG_FILES):
	@$(MAKE) $(MAKECMDGOALS) -C $(dir $@) NODISP=1

$(GROUP_TIME_FILE): $(TEST_TIME_FILES)
	@$(call group_log,$^,$@)

$(TEST_TIME_FILES):
	@$(MAKE) time -sC $(dir $@)
