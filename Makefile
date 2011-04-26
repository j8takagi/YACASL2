.PHONY: build check doc doxygen clean gtags htags clean_htags
MKDIR = mkdir -p
MV = mv
DOXYGEN = doxygen
GTAGS = gtags
HTAGS = htags
HTAGSFLAG = -anosx

all: build gtags

build:
	$(MAKE) -C src

check:
	@$(MAKE) -sC test/system

doc:
	@$(MAKE) -sC doc

doxygen: doc_inner
	@$(DOXYGEN)

gtags: clean_gtags
	@$(GTAGS)

htags: doc_inner
	@$(MAKE) clean_htags
	@$(MAKE) gtags
	@$(HTAGS) $(HTAGSFLAG) $<
	@$(MV) $</HTML $</htags

doc_inner:
	@$(MKDIR) $@

clean: clean_cmd clean_src clean_gtags clean_systemtest clean_unittest clean_doc clean_doc_inner

clean_cmd:
	@rm -f casl2 comet2 dumpword

clean_src:
	@$(MAKE) -sC src clean

clean_gtags:
	@rm -f GPATH GRTAGS GSYMS GTAGS

clean_systemtest:
	@$(MAKE) -sC test/system/casl2 clean
	@$(MAKE) -sC test/system/comet2 clean
	@$(MAKE) -sC test/system/dumpword clean

clean_unittest:
	@$(MAKE) -sC test/unit clean

clean_doc:
	@$(MAKE) -sC doc clean

clean_doc_inner:
	@rm -rf doc_inner

clean_htags:
	@rm -rf doc_inner/htags

clean_doxygen:
	@rm -rf doc_inner/doxygen
