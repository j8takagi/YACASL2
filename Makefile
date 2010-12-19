.PHONY: build check doc clean

build:
	make -C src

check:
	@make -sC test/system

doc:
	make -sC doc

clean:
	@make -sC src $@
	@make -sC test/system $@
	@make -sC test/unit $@
	@rm -f casl2 comet2 dumpword
