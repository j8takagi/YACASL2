# 複数の子ディレクトリーでmakeを実行
CMD = casl2_smoke casl2_opt casl2_cmd casl2_err casl2_lib comet2_smoke comet2_opt comet2_cmd comet2_err comet2_lib dumpword
define make_dirs
    $(foreach d,$1,$(MAKE) -sC $d $2)
endef

.PHONY: check clean

check:
	@$(call make_dirs,$(CMD),$@; )

clean:
	@$(call make_dirs,$(CMD),$@; )
