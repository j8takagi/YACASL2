.PHONY: build check doc clean

build:
	$(MAKE) -C src

check:
	@$(MAKE) -sC test/system

doc:
	@$(MAKE) -sC doc

clean:
	@$(MAKE) -sC src clean
	@$(MAKE) -sC test/system/casl2 clean
	@$(MAKE) -sC test/system/comet2 clean
	@$(MAKE) -sC test/system/dumpword clean
	@$(MAKE) -sC test/unit clean
	@rm -f casl2 comet2 dumpword
