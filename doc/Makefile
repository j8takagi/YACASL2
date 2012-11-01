MANUAL = manual
INSTALLDOC = install
MANUALSRC = $(MANUAL).texi
INSTALLDOCSRC = $(INSTALLDOC).texi
CSS = style.css
MANUALHTMLDIR = manual_html

MV ?= mv
CP ?= cp
MKDIR ?= mkdir
RMRF ?= rm -rf
PTEX ?= ptex
TEXI2DVI ?= texi2dvi
DVI2PDF ?= dvipdfmx
MAKEINFO ?= gmakeinfo
INSTALL ?= install
INSTALL-INFO ?= ginstall-info
GZIP = gzip

prefix ?= ~
infodir ?= $(prefix)/share/info

.PHONY: all install-manual uninstall-manual clean clean-manual clean-installdoc clean-textmp

.INTERMEDIATE: $(MANUAL).dvi

all: $(MANUAL).info $(MANUAL).html $(MANUALHTMLDIR) $(MANUAL).pdf $(INSTALLDOC).html ../INSTALL

$(MANUALHTMLDIR): $(MANUALSRC) $(CSS)
	$(INSTALL) -d $@
	$(INSTALL) $(CSS) $@/
	$(MAKEINFO) -o $@ --html --css-ref=$(CSS) $<

$(MANUAL).html: $(MANUALSRC) $(CSS)
	$(MAKEINFO) -o $@ --no-split --html --css-include=$(CSS) $<

$(MANUAL).pdf: $(MANUAL).dvi
	$(DVI2PDF) $<

$(MANUAL).dvi: $(MANUALSRC)
	TEX=$(PTEX) $(TEXI2DVI) -q --texinfo=@afourpaper -o $@ $<
	@$(MAKE) clean-textmp

$(MANUAL).txt: $(MANUALSRC)
	$(MAKEINFO) --no-headers -o $@ $(MANUALSRC)

install-manual: $(MANUAL).info
	$(INSTALL) -d $(infodir)
	$(INSTALL) $(MANUAL).info $(infodir)/
	$(INSTALL-INFO) $(infodir)/$(MANUAL).info $(infodir)/dir
	$(GZIP) $(infodir)/$(MANUAL).info

uninstall-manual: $(MANUAL).info
	$(INSTALL-INFO) --delete $(infodir)/$(MANUAL) $(infodir)/dir
	$(RM) $(infodir)/$(MANUAL)*

$(INSTALLDOC).html: $(INSTALLDOCSRC) $(CSS)
	$(MAKEINFO) -o $@ --no-split --html --css-include=$(CSS) $<

../INSTALL: $(INSTALLDOCSRC)
	$(MAKEINFO) -o $@ --no-headers $<

clean: clean-manual clean-installdoc clean-textmp

clean-manual:
	@$(RMRF) html $(MANUALHTMLDIR) $(MANUAL).info $(MANUAL).pdf $(MANUAL).html $(MANUAL).txt 

clean-textmp:
	@$(RMRF) $(MANUAL).aux $(MANUAL).cp $(MANUAL).cps $(MANUAL).fn $(MANUAL).ky $(MANUAL).log $(MANUAL).pg $(MANUAL).pgs $(MANUAL).tmp $(MANUAL).toc $(MANUAL).tp $(MANUAL).vr

clean-installdoc:
	@$(RMRF) install.html ../INSTALL
