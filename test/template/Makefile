# autotest.mk > test_template > Makefile
# テストグループ作成のMakefile
#
# オペレーター
# make         : テストグループディレクトリーを作成し、Makefile群をコピー
# make group   : ↓
# make groupdir: テストグループディレクトリーを作成
# make copymk  : Makefile群をコピー
# make linkmk  : Makefile群をコピー

# 変数やマクロの定義
include Define.mk

# テストグループのMakefileとしてコピーされるファイル
GROUP_MAKEFILE := Group.mk

.PHONY: group groupdir copymk linkmk

MKFILE_METHOD = $(if $(LINKMK),linkmk,copymk)

group: groupdir $(MKFILE_METHOD)

groupdir:
	@$(MKDIR) $(GROUPDIR)

copymk:
	@$(CP) $(GROUP_MAKEFILE) $(GROUPDIR)/Makefile
	@$(CP) $(MAKEFILES) $(GROUPDIR)/

linkmk:
	@$(LN) $(CURDIR)/$(GROUP_MAKEFILE) $(GROUPDIR)/Makefile
	@$(LN) $(MAKEFILES_ABS) $(GROUPDIR)/
