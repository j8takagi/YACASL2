.PHONY: all srcall gtags \
        check valgrind \
        doc alldoc doc_inner \
        install uninstall \
        version gittag copyright \
        gitpush gitpushorigin gitpushgithub \
        clean src-clean gtags-clean \
        test-clean doc-clean doc_inner-clean

CAT := cat
CP := cp -v
DATE := date
DIFF :=diff
ECHO := /bin/echo
EXPR := expr
GIT := git
GREP := grep
GTAGS := gtags
INSTALL := install
SED := sed
WC := wc
WHICH := which
XARGS := xargs

prefix ?= ~
bindir ?= $(prefix)/bin
libdir ?= $(prefix)/share

VERSION := $(shell $(CAT) VERSION)

VERSIONGITREF = $(shell $(GIT) show-ref -s --tags $(VERSION))

MAINGITREF = $(shell $(GIT) show-ref -s refs/heads/main)

VERSIONFILES := include/version.h \
        test/system/casl2_opt/opt_v/0.txt \
        test/system/comet2_opt/opt_v/0.txt \
        test/system/dumpword/opt_v/0.txt

CMDFILES := casl2 comet2 dumpword casl2rev comet2monitor

YEAR = $(shell $(DATE) '+%Y')

all: copyright INSTALL build gtags

build: version
	$(MAKE) -C src all
	@(for f in $(CMDFILES); do if test ! -e $$f -o src/$$f -nt $$f; then $(CP) src/$$f $$f; fi; done)

INSTALL: doc/install.txt
	$(CP) $< $@

doc/install.txt:
	$(MAKE) -C doc base

alldoc:
	$(MAKE) -C doc all

doc_inner:
	$(MAKE) -C doc_inner all

check:
	$(MAKE) -sC test/system

smoke:
	$(MAKE) -sC test/system smoke

smoke-valgrind:
	$(MAKE) -sC test/system smoke-valgrind

valgrind:
	$(MAKE) -sC test/system valgrind

install: $(CMDFILES) install-info
	$(INSTALL) -d $(bindir)
	$(INSTALL) $(CMDFILES) $(bindir)/

install-info:
	$(MAKE) -C doc install-info

uninstall: uninstall-info
	$(RM) $(prefix $(bindir)/,$(CMDFILES))

version: version___stamp

version___stamp: $(VERSIONFILES)
	@$(PRINTF) "YACASL2 Version: %s\n" $(VERSION) >$@

$(VERSIONFILES): VERSION
	@$(SED) -e "s/@@VERSION@@/$(VERSION)/g" $@.version >$@

copyright: LICENSE.copyright___stamp  README.copyright___stamp

%.copyright___stamp: %
	$(SED) -i.sedbak 's/Copyright (c) 2010-20[0-9][0-9]/Copyright (c) 2010-$(YEAR)/g' $< && $(DIFF) $<.sedbak $< >$@ && $(RM) *.sedbak

include git.mk

gittag: gittag___stamp

gitpush: gitpush___stamp

distclean: cmd-clean src-distclean gitclean version-clean clean

clean: src-clean doc-clean doc_inner-clean

cmd-clean:
	$(RM) $(CMDFILES)

src-clean:
	$(MAKE) -sC src clean

src-distclean:
	$(MAKE) -sC src distclean

doc-clean:
	$(MAKE) -sC doc clean

doc_inner-clean:
	$(MAKE) -sC doc_inner clean

version-clean:
	$(RM) $(VERSIONFILES)

test-clean:
	$(MAKE) -sC test clean
