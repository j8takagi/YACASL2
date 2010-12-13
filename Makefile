CLEANDIR = 

.PHONY: build check doc clean

build:
	make -C src

check:
	@make -sC test/system

doc:
	@make -sC doc

clean:
	@for target in src test/integration test/unit as/casl2lib; do $(MAKE) -sC $$target clean; done;
	@rm -f casl2 comet2 dumpword
