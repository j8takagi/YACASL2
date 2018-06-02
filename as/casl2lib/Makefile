.PHONY: all libfiles install-casl2lib uninstall-casl2lib

CAT := cat
CASL2OUT := casl2 -M4096 -O
RMF := rm -f
RMDIR := rmdir
ECHO := echo
INSTALL := install

prefix ?= ~
casl2libdir ?= $(prefix)/share/casl2lib

all: stdlib.casl libfiles

%.casl: src/%.casl
	$(CAT) $^ >$@

%.o: %.casl
	$(CASL2OUT)$< $@

include casl2lib.dep

clean:

distclean:
	$(RM) *.casl

install-casl2lib: *.casl
	$(INSTALL) -d $(casl2libdir)
	$(INSTALL) $^ $(casl2libdir)/

uninstall-casl2lib:
	@$(RMF) $(casl2libdir)/*.casl
	@$(RMDIR) $(casl2libdir) || if test -e $(casl2libdir); then $(ECHO) "$(casl2libdir): files may be added by others."; fi
