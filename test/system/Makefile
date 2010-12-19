# 複数の子ディレクトリーでmakeを実行
CMDS = casl2 comet2 dumpword
define make_dirs
    $(foreach d,$1,$(MAKE) -sC $d $2)
endef

check:
	@$(call make_dirs,$(CMDS),$@; )

clean:
	@$(call make_dirs,$(CMDS),$@; )
