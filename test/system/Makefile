# 複数の子ディレクトリーでmakeを実行
CMD = casl2_opt casl2_cmd casl2_err comet2_smoke comet2_opt comet2_cmd comet2_err casl2rev dumpword
define make_dirs
    $(foreach d,$1,$(MAKE) -sC $d $2)
endef

.PHONY: check smoke valgrind clean

check:
	@$(call make_dirs,$(CMD),$@; )

smoke:
	$(MAKE) -sC casl2_smoke check

smoke-valgrind:
	$(MAKE) -sC casl2_smoke valgrind

valgrind:
	@$(call make_dirs,casl2_smoke $(CMD),$@; )

clean:
	@$(call make_dirs,casl2_smoke $(CMD),$@; )
