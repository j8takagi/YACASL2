.PHONY: all install-casl2lib uninstall-casl2lib

CAT := cat
RMF := rm -f
RMDIR := rmdir
ECHO := echo
INSTALL := install

prefix ?= ~
casl2libdir ?= $(prefix)/share/casl2lib

all: stdlib.casl abs.casl addl32.casl diva.casl divl.casl inl.casl l2str.casl max.casl minim.casl mula.casl mull.casl outa.casl outa_q15.casl outb.casl outb_w.casl outl.casl outl_q15.casl outpbm.casl pbm16.casl str2l.casl

%.casl: src/%.casl
	$(CAT) $^ >$@

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
