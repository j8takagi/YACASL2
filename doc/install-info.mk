prefix ?= ~
infodir ?= $(prefix)/share/info

GZIP := gzip
INSTALL := install
INSTALL-INFO := install-info

.PHONY: install-info uninstall-info

install-info: $(INSTALL-INFO-TARGETS)
	$(INSTALL) -d $(infodir)
	$(INSTALL) $< $(infodir)/
	$(INSTALL-INFO) $(infodir)/$(notdir $<) $(infodir)/dir
	$(GZIP) -f $(infodir)/$(notdir $<)

uninstall-info:
	$(INSTALL-INFO) --delete $(addprefix $(infodir)/,$(notdir $(INSTALL-INFO-TARGETS))) $(infodir)/dir
	$(RM) $(addprefix $(infodir)/,$(notdir $(INSTALL-INFO-TARGETS)))
