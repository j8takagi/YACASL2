.PHONY: install-casl2lib uninstall-casl2lib

RMF ?= rm -f
RMDIR ?= rmdir
ECHO ?= echo
INSTALL ?= install

prefix ?= ~
sharedir ?= $(prefix)/share

install-casl2lib: *.casl
	$(INSTALL) -d $(sharedir)/casl2lib
	$(INSTALL) $^ $(sharedir)/casl2lib/

uninstall-casl2lib:
	@$(RMF) $(sharedir)/casl2lib/*.casl
	@$(RMDIR) $(sharedir)/casl2lib || if test -e $(sharedir)/casl2lib; then $(ECHO) "$(sharedir)/casl2lib: files may be added by others."; fi
