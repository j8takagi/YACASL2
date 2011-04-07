.PHONY: build check doc doxygen clean gtags htags
MKDIR = mkdir -p
MV = mv
DOXYGEN = doxygen
GTAGS = gtags
HTAGS = htags
HTAGSFLAG = -anosx

build:
	$(MAKE) -C src

check:
	@$(MAKE) -sC test/system

doc:
	@$(MAKE) -sC doc

doxygen: doc_inner
	@$(DOXYGEN)

gtags:
	@$(GTAGS)

htags: doc_inner gtags
	@$(HTAGS) $(HTAGSFLAG) $</

doc_inner:
	@$(MKDIR) $@

clean: clean_cmd clean_src clean_gtags clean_systemtest clean_unittest clean_doc_inner clean_doc

clean_cmd:
	@rm -f casl2 comet2 dumpword

clean_gtags:
	@rm -f GPATH GRTAGS GSYMS GTAGS

clean_src:
	@$(MAKE) -sC src clean

clean_systemtest:
	@$(MAKE) -sC test/system/casl2 clean
	@$(MAKE) -sC test/system/comet2 clean
	@$(MAKE) -sC test/system/dumpword clean

clean_unittest:
	@$(MAKE) -sC test/unit clean

clean_doc_inner:
	@rm -rf doc_inner

clean_doc:
	@$(MAKE) -sC doc clean
