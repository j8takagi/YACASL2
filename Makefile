.PHONY: build check doc doxygen clean gtags htags
MKDIR = mkdir -p
DOXYGEN = doxygen
GTAGS = gtags
HTAGS = htags
HTAGSFLAG = --map-file -anos --tree-view

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

htags: doc/doxygen/html gtags
	@$(HTAGS) $(HTAGSFLAG) $</

doc/doxygen/html:
	@$(MKDIR) $@

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
