# 複数の子ディレクトリーでmakeを実行
CMD = casl2 comet2 dumpword
define make_dirs
    $(foreach d,$1,$(MAKE) -sC $d $2)
endef

.PHONY: check clean

check:
	@$(call make_dirs,$(CMD),$@; )

clean:
	@$(call make_dirs,$(CMD),$@; )
