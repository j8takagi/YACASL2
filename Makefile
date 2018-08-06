.PHONY: all srcall gtags \
        check valgrind \
        doc alldoc doc_inner \
        install uninstall \
        version gittag \
        clean src-clean gtags-clean \
        test-clean doc-clean doc_inner-clean

CAT := cat
CP := cp -v
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

VERSIONGITREF := $(shell $(GIT) show-ref -s --tags $(VERSION))

MASTERGITREF := $(shell $(GIT) show-ref -s refs/heads/master)

VERSIONFILES := include/version.h \
        test/system/casl2_opt/opt_v/0.txt \
        test/system/comet2_opt/opt_v/0.txt \
        test/system/dumpword/opt_v/0.txt

CMDFILES := casl2 comet2 dumpword casl2rev comet2monitor

all: build INSTALL gtags

build:
	$(MAKE) -C src all
	@(for f in $(CMDFILES); do if test ! -e $$f -o src/$$f -nt $$f; then $(CP) src/$$f $$f; fi; done)

gtags:
	$(if $(strip $(shell $(WHICH) $(GTAGS))),$(GTAGS),@$(ECHO) '$(GTAGS): not found')

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

valgrind:
	$(MAKE) -sC test/system valgrind

install: casl2 comet2 dumpword install-info
	$(INSTALL) -d $(bindir)
	$(INSTALL) $(CMD) $(bindir)/

install-info:
	$(MAKE) -C doc install-info

uninstall: uninstall-info
	$(RM) $(prefix $(bindir)/,$(CMD))

version: $(VERSIONFILES)
	@$(ECHO) "YACASL2 Version: $(VERSION)"

$(VERSIONFILES): VERSION
	@$(SED) -e "s/@@VERSION@@/$(VERSION)/g" $@.version >$@

gittag:
	if test `$(GIT) status -s | $(WC) -l` -gt 0; then $(ECHO) "Error: commit, first."; exit 1; fi; if test "$(VERSIONGITREF)" != "$(MASTERGITREF)"; then $(GIT) tag $(VERSION); fi

distclean: cmd-clean src-distclean gtags-clean version-clean clean

clean: src-clean doc-clean doc_inner-clean

cmd-clean:
	$(RM) $(CMD)

src-clean:
	$(MAKE) -sC src clean

src-distclean:
	$(MAKE) -sC src distclean

gtags-clean:
	$(RM) GPATH GRTAGS GSYMS GTAGS

doc-clean:
	$(MAKE) -sC doc clean

doc_inner-clean:
	$(MAKE) -sC doc_inner clean

version-clean:
	$(RM) $(VERSIONFILES)

test-clean:
	$(MAKE) -sC test clean
