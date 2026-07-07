.PHONY: all gtags check valgrind doc doc_inner copyright install uninstall patchup___stamp gittag gitpush clean src-clean gtags-clean test-clean doc-clean doc_inner-clean

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
PRINTF := printf
SED := sed
WC := wc
WHICH := which
XARGS := xargs

prefix ?= ~
bindir ?= $(prefix)/bin
libdir ?= $(prefix)/share

VERSIONFILES := include/version.h test/system/casl2_opt/opt_v/0.txt test/system/comet2_opt/opt_v/0.txt test/system/dumpword/opt_v/0.txt

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

doc:
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

$(VERSIONFILES): VERSION
	@$(SED) -e "s/@@VERSION@@/$(VERSION)/g" $@.version >$@

copyright: LICENSE.copyright___stamp  README.copyright___stamp

%.copyright___stamp: %
	$(SED) -i.sedbak 's/Copyright (c) 2010-20[0-9][0-9]/Copyright (c) 2010-$(YEAR)/g' $< && $(DIFF) $<.sedbak $< >$@ && $(RM) *.sedbak

gtags:
	$(if $(strip $(shell $(WHICH) $(GTAGS))),$(GTAGS),@$(ECHO) '$(GTAGS): not found')

gittag: gittag___stamp

gitpush: gitpush___stamp


gitpush___stamp: gittag___stamp
	$(GIT) push origin main --tags
	$(GIT) push github main --tags
	$(GIT) rev-parse HEAD > $@

gittag___stamp: patchup___stamp
	$(GIT) tag --points-at HEAD | $(GREP) -qx "$$($(CAT) VERSION)" || $(GIT) tag $$($(CAT) VERSION) main
	$(PRINTF) "%s\n" "$$($(CAT) VERSION)" > $@

patchup___stamp:
	$(GIT) diff-index --quiet HEAD -- || ($(PRINTF) "Error: commit, first.\n"; exit 1)
# VERSION FORMAT: v<VERSIONNO>p<PATCHNO> (Example: v0.1p0)
	$(GIT) tag --points-at HEAD | $(GREP) -qx "$$(cat VERSION)" && exit 0 || while $(GIT) rev-parse -q --verify $$(cat VERSION) >/dev/null 2>&1; do $(PRINTF) "v%.1fp%02d\n" $$($(SED) 's/v\([0-9]*\.[0-9]*\)p.*$$/\1/' VERSION) $$($(EXPR) $$($(SED) 's/^.*p//' VERSION)  + 1) >VERSION; done;
	$(GIT) diff-index --quiet HEAD -- || $(GIT) commit -a -m "version up to $$($(CAT) VERSION)"
	$(PRINTF) "%s\n" "$$($(CAT) VERSION)" >$@

distclean: cmd-clean src-distclean gitclean version-clean clean

clean: stamp-clean src-clean doc-clean doc_inner-clean

stamp-clean:
	$(RM) *___stamp

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
