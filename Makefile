.PHONY: all build gtags check doc info html doc-inner install uninstall install-info uninstall-info install-casl2lib uninstall-casl2lib version gittag clean clean-src clean-gtags clean-test clean-doc clean-doc-inner

GTAGS := gtags
RMF := rm -f
WHICH := which
ECHO := echo
INSTALL := install
SED := sed
CAT := cat
XARGS := xargs
GITTAG := git tag

prefix ?= ~
bindir ?= $(prefix)/bin

VERSIONFILES = include/package.h test/system/casl2/opt_v/0.txt test/system/comet2/opt_v/0.txt test/system/dumpword/opt_v/0.txt

all: build docall gtags

build:
	$(MAKE) -C src all

gtags:
	@$(WHICH) $(GTAGS) >/dev/null && $(GTAGS) || $(ECHO) "$(GTAGS): not found"

docall:
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

version: VERSION $(VERSIONFILES)
	@$(ECHO) "YACASL2 Version:" `$(CAT) VERSION`

$(VERSIONFILES): VERSION
	$(SED) -e "s/@@VERSION@@/`cat VERSION`/g" $@.version >$@

gittag: VERSION
	$(CAT) $@ | $(XARGS) $(GITTAG)

clean: clean-src clean-gtags clean-doc clean-doc-inner clean-version

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

clean-version:
	@$(RMF) $(VERSIONFILES)
