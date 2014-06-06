.PHONY: all build gtags \
        check \
        doc alldoc doc_inner \
        install uninstall \
        version gittag \
        clean src-clean gtags-clean \
        test-clean doc-clean doc_inner-clean

CMD := casl2 comet2 dumpword

CAT := cat
CP := cp
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

VERSION := $(shell $(CAT) VERSION)

VERSIONGITREF := $(shell $(GIT) show-ref -s --tags $(VERSION))

MASTERGITREF := $(shell $(GIT) show-ref -s refs/heads/master)

VERSIONFILES = include/package.h \
        test/system/casl2/opt_v/0.txt \
        test/system/comet2/opt_v/0.txt \
        test/system/dumpword/opt_v/0.txt

all: build doc gtags

build:
	$(MAKE) -C src all
	$(CP) $(addprefix src/,$(CMD)) ./

gtags:
	$(if $(strip $(shell $(WHICH) $(GTAGS))),$(GTAGS),@$(ECHO) '$(GTAGS): not found')

doc:
	$(MAKE) -C doc base
	$(MAKE) INSTALL

INSTALL: doc/install.txt
	$(CP) $< $@

alldoc:
	$(MAKE) -C doc all

doc_inner:
	$(MAKE) -C doc_inner all

check:
	$(MAKE) -sC test/system

install: casl2 comet2 dumpword install-info install-casl2lib
	$(INSTALL) -d $(bindir)
	$(INSTALL) $(CMD) $(bindir)/

uninstall: uninstall-info uninstall-casl2lib
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
