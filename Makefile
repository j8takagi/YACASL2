.PHONY: build check doc doxygen clean gtags htags
DOXYGEN = doxygen
GTAGS = gtags
HTAGS = htags
HTAGSFLAG = --map-file
build:
	$(MAKE) -C src

check:
	@$(MAKE) -sC test/system

doc:
	@$(MAKE) -sC doc

doxygen: htags
	@$(DOXYGEN)

gtags:
	@$(GTAGS)

htags: gtags
	@$(HTAGS) $(HTAGSFLAG) doc/doxygen/html/

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
