MANUAL = manual
INSTALLDOC = install
MANUALSRC = $(MANUAL).texi
MANUALINFO = yacasl2.info
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

.PHONY: all info html install-info uninstall-info clean clean-manual clean-installdoc clean-textmp

.INTERMEDIATE: $(MANUAL).dvi

all: info html $(MANUAL).pdf ../INSTALL

info: $(MANUALINFO)

$(MANUALINFO): $(MANUALSRC)
	$(MAKEINFO) -o $(MANUALINFO) $(MANUALSRC)

html: $(MANUAL).html $(MANUALHTMLDIR) $(INSTALLDOC).html

$(MANUAL).html: $(MANUALSRC) $(CSS)
	$(MAKEINFO) -o $@ --no-split --html --css-include=$(CSS) $<

$(MANUALHTMLDIR): $(MANUALSRC) $(CSS)
	$(INSTALL) -d $@
	$(INSTALL) $(CSS) $@/
	$(MAKEINFO) -o $@ --html --css-ref=$(CSS) $<

$(MANUAL).pdf: $(MANUAL).dvi
	$(DVI2PDF) $<

$(MANUAL).dvi: $(MANUALSRC)
	TEX=$(PTEX) $(TEXI2DVI) -q --texinfo=@afourpaper -o $@ $<
	@$(MAKE) clean-textmp

$(MANUAL).txt: $(MANUALSRC)
	$(MAKEINFO) --no-headers --disable-encoding -o $@ $(MANUALSRC)

install-info: $(MANUAL).info
	$(INSTALL) -d $(infodir)
	$(INSTALL) $(MANUALINFO) $(infodir)/
	$(INSTALL-INFO) $(infodir)/$(MANUALINFO) $(infodir)/dir
	$(GZIP) -f $(infodir)/$(MANUALINFO)

uninstall-info: $(MANUAL).info
	$(INSTALL-INFO) --delete $(infodir)/$(MANUAL) $(infodir)/dir
	$(RM) $(infodir)/$(MANUAL)*

$(INSTALLDOC).html: $(INSTALLDOCSRC) $(CSS)
	$(MAKEINFO) -o $@ --no-split --html --css-include=$(CSS) $<

../INSTALL: $(INSTALLDOCSRC)
	$(MAKEINFO) -o $@ --no-headers --disable-encoding $<

clean: clean-manual clean-installdoc clean-textmp

clean-manual:
	@$(RMRF) html $(MANUALHTMLDIR) $(MANUAL).info $(MANUAL).pdf $(MANUAL).html $(MANUAL).txt 

clean-textmp:
	@$(RMRF) $(MANUAL).aux $(MANUAL).cp $(MANUAL).cps $(MANUAL).fn $(MANUAL).ky $(MANUAL).log $(MANUAL).pg $(MANUAL).pgs $(MANUAL).tmp $(MANUAL).toc $(MANUAL).tp $(MANUAL).vr

clean-installdoc:
	@$(RMRF) install.html ../INSTALL
