.PHONY: all build gtags \
        check \
        doc info html doc-inner \
        install uninstall \
        install-info uninstall-info \
        install-casl2lib uninstall-casl2lib \
        version gittag \
        clean src-clean gtags-clean \
        test-clean doc-clean doc-inner-clean

CMD := casl2 comet2 dumpword

CAT := cat
CP := cp
ECHO := echo
GITTAG := git tag
GTAGS := gtags
INSTALL := install
SED := sed
WHICH := which

prefix ?= ~
bindir ?= $(prefix)/bin

VERSION = $(shell $(CAT) VERSION)
VERSIONFILES = include/package.h test/system/casl2/opt_v/0.txt test/system/comet2/opt_v/0.txt test/system/dumpword/opt_v/0.txt

all: build doc gtags

build: version
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
	$(INSTALL) $(CMD) $(bindir)/

uninstall: uninstall-info uninstall-casl2lib
	$(RM) $(prefix $(bindir)/,$(CMD))

install-info:
	$(MAKE) -C doc install-info

uninstall-info:
	$(MAKE) -C doc uninstall-info

install-casl2lib:
	$(MAKE) -C as/casl2lib install-casl2lib

uninstall-casl2lib:
	$(MAKE) -C as/casl2lib uninstall-casl2lib

version: $(VERSIONFILES)
	@$(ECHO) "YACASL2 Version: $(VERSION)"

$(VERSIONFILES): VERSION
	@$(SED) -e "s/@@VERSION@@/$(VERSION)/g" $@.version >$@

gittag: VERSION
	$(GITTAG) $(VERSION)

clean: cmd-clean src-clean gtags-clean doc-clean doc-inner-clean version-clean

cmd-clean:
	@$(RM) $(CMD)

src-clean:
	@$(MAKE) -sC src clean

gtags-clean:
	@$(RM) GPATH GRTAGS GSYMS GTAGS

doc-clean:
	@$(MAKE) -sC doc clean

doc-inner-clean:
	@$(MAKE) -sC doc_inner clean

version-clean:
	@$(RM) $(VERSIONFILES)

test-clean:
	@$(MAKE) -sC test clean
