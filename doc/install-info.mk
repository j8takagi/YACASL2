prefix ?= ~
infodir ?= $(prefix)/share/info

GZIP := gzip
INSTALL := install
INSTALL-INFO := install-info

.PHONY: install-info uninstall-info

install-info: yacasl2.info
	$(INSTALL) -d $(infodir)
	$(INSTALL) $< $(infodir)/
	$(INSTALL-INFO) $(infodir)/$(notdir $<) $(infodir)/dir
	$(GZIP) -f $(infodir)/$(notdir $<)

uninstall-info:
	$(INSTALL-INFO) --delete $(addprefix $(infodir)/,$(notdir $(TARGET))) $(infodir)/dir
	$(RM) $(addprefix $(infodir)/,$(notdir $(TARGET)))
