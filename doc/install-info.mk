prefix ?= ~
infodir ?= $(prefix)/share/info

GZIP := gzip
INSTALL := install
INSTALL-INFO := install-info

.PHONY: install-info uninstall-info

install-info: yacasl2.info
	$(INSTALL) -d $(infodir)
	$(INSTALL) $< $(infodir)/
	$(INSTALL-INFO) $(infodir)/$< $(infodir)/dir
	$(GZIP) -f $(infodir)/$<

uninstall-info:
	$(INSTALL-INFO) --delete $(addprefix $(infodir)/,$(TARGETS)) $(infodir)/dir
	$(RM) $(addprefix $(infodir)/,$(TARGETS))
