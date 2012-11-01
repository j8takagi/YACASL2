.PHONY: install-casl2lib uninstall-casl2lib

RMF ?= rm -f
RMDIR ?= rmdir
ECHO ?= echo
INSTALL ?= install

prefix ?= ~
casl2libdir ?= $(prefix)/lib/yacasl2

install-casl2lib: *.casl
	$(INSTALL) -d $(casl2libdir)
	$(INSTALL) $^ $(casl2libdir)/

uninstall-casl2lib:
	@$(RMF) $(casl2libdir)/*.casl
	@$(RMDIR) $(casl2libdir) || if test -e $(casl2libdir); then $(ECHO) "$(casl2libdir): files may be added by others."; fi
