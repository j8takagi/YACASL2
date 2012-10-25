.PHONY: all build gtags check doc doc_inner clean clean_src clean_gtags clean_test clean_doc clean_doc_inner

GTAGS ?= gtags
RMF ?= rm -f
WHICH ?= which
ECHO ?= echo

all: build doc doc_inner

build:
	$(MAKE) -C src

gtags:
	$(WHICH) $(GTAGS) && $(GTAGS) >/dev/null || $(ECHO) "$(GTAGS): not found"

check:
	$(MAKE) -C test/system

doc:
	$(MAKE) -C $@

doc_inner:
	$(MAKE) -C $@

clean: clean_src clean_gtags clean_test clean_doc clean_doc_inner

clean_src:
	@$(MAKE) -sC src clean

clean_gtags:
	@$(RMF) GPATH GRTAGS GSYMS GTAGS

clean_test:
	@$(MAKE) -sC test clean

clean_doc:
	@$(MAKE) -sC doc clean

clean_doc_inner:
	@$(MAKE) -sC doc_inner clean
