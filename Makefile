.PHONY: all build gtags check doc info html doc-inner install uninstall install-info uninstall-info install-casl2lib uninstall-casl2lib clean clean-src clean-gtags clean-test clean-doc clean-doc-inner

GTAGS ?= gtags
RMF ?= rm -f
WHICH ?= which
ECHO ?= echo
INSTALL ?= install

prefix ?= ~
bindir ?= $(prefix)/bin
casl2libdir ?= $(prefix)/yacasl2/casl2lib

all: build info html gtags

build:
	$(MAKE) -C src all

gtags:
	@$(WHICH) $(GTAGS) >/dev/null && $(GTAGS) || $(ECHO) "$(GTAGS): not found"

docall: doc
	$(MAKE) -C doc all

info:
	$(MAKE) -C doc info

html:
	$(MAKE) -C doc html

doc-inner:
	$(MAKE) -C doc_inner all

check:
	$(MAKE) -sC test/system

install: casl2 comet2 dumpword install-info install-casl2lib
	$(INSTALL) -d $(bindir)
	$(INSTALL) casl2 comet2 dumpword $(bindir)/

uninstall: uninstall-info uninstall-casl2lib
	$(RMF) $(bindir)/casl2 $(bindir)/comet2 $(bindir)/dumpword

install-info:
	$(MAKE) -C doc install-info

uninstall-info:
	@$(MAKE) -C doc uninstall-info

install-casl2lib:
	$(MAKE) -C as/casl2lib install-casl2lib

uninstall-casl2lib:
	@$(MAKE) -C as/casl2lib uninstall-casl2lib

clean: clean-src clean-gtags clean-doc clean-doc-inner

clean-src:
	@$(MAKE) -sC src clean

clean-gtags:
	@$(RMF) GPATH GRTAGS GSYMS GTAGS

clean-doc:
	@$(MAKE) -sC doc clean

clean-doc-inner:
	@$(MAKE) -sC doc_inner clean

clean-test:
	@$(MAKE) -sC test clean
