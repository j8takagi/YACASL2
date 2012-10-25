.PHONY: all build gtags check doc doc_inner clean clean_cmd clean_gtags clean_src clean_test clean_doc clean_doc_inner
GTAGS ?= gtags
RMF ?= rm -f
all: build gtags doc

build:
	$(MAKE) -C src

gtags:
	@(which $(GTAGS) && $(GTAGS)) >/dev/null

check:
	@$(MAKE) -sC test/system

doc:
	@$(MAKE) -sC $@

doc_inner:
	@$(MAKE) -sC $@

clean: clean_cmd clean_gtags clean_src clean_test clean_doc clean_doc_inner

clean_cmd:
	@$(RMF) casl2 comet2 dumpword

clean_gtags:
	@$(RMF) GPATH GRTAGS GSYMS GTAGS

clean_src:
	@$(MAKE) -sC src clean

clean_test:
	@$(MAKE) -sC test clean

clean_doc:
	@$(MAKE) -sC doc clean

clean_doc_inner:
	@$(MAKE) -sC doc_inner clean
