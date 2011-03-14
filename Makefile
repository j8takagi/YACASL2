.PHONY: build check doc doxygen clean gtags htags
DOXYGEN = doxygen
GTAGS = gtags
HTAGS = htags

build:
	$(MAKE) -C src

check:
	@$(MAKE) -sC test/system

doc:
	@$(MAKE) -sC doc

doxygen:
	@$(DOXYGEN)

gtags:
	@$(GTAGS)

htags:
	@$(HTAGS)

clean:
	@$(MAKE) -sC src clean
	@$(MAKE) -sC test/system/casl2 clean
	@$(MAKE) -sC test/system/comet2 clean
	@$(MAKE) -sC test/system/dumpword clean
	@$(MAKE) -sC test/unit clean
	@rm -rf doc/doxygen/*
	@rm -f casl2 comet2 dumpword
	@rm -f GPATH GRTAGS GSYMS GTAGS
	@rm -rf HTML
