DOCNAME = yacasl2
SRC = $(DOCNAME).texi
CSS = style.css
HTMLDIR = html

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

prefix ?= ~
infodir ?= $(prefix)/share/info

.PHONY: all info htmlnosplit html pdf install-info clean clean-docs clean-tex-tmp

.INTERMEDIATE: $(DOCNAME).dvi

all: info html htmls pdf

info: $(DOCNAME).info

html: $(DOCNAME).html
	$(MV) $< index.html

htmls: $(SRC) $(CSS)
	$(INSTALL) -d $(HTMLDIR)
	$(INSTALL) $(CSS) $(HTMLDIR)/
	$(MAKEINFO) -o $(HTMLDIR) --html --css-ref=$(CSS) $(SRC)

pdf: $(DOCNAME).pdf

$(DOCNAME).html: $(SRC) $(CSS)
	$(MAKEINFO) -o $@ --no-split --html --css-include=$(CSS) $(SRC)

$(DOCNAME).pdf: $(DOCNAME).dvi
	$(DVI2PDF) $^

$(DOCNAME).dvi: $(SRC)
	TEX=$(PTEX) $(TEXI2DVI) -q --texinfo=@afourpaper -o $@ $<
	@$(MAKE) clean-tex-tmp

install-info: $(DOCNAME).info
	$(INSTALL) -d $(infodir)
	$(INSTALL) $(DOCNAME).info $(infodir)/$(DOCNAME)
	$(INSTALL-INFO) $(infodir)/$(DOCNAME) $(infodir)/dir

uninstall-info: $(DOCNAME).info
	$(INSTALL-INFO) --delete $(infodir)/$(DOCNAME) $(infodir)/dir
	$(RM) $(infodir)/$(DOCNAME)

clean: clean-docs clean-tex-tmp

clean-docs:
	@$(RMRF) html $(DOCNAME).info $(DOCNAME).pdf index.html

clean-tex-tmp:
	@$(RMRF) $(DOCNAME).aux $(DOCNAME).cp $(DOCNAME).cps $(DOCNAME).fn $(DOCNAME).ky $(DOCNAME).log $(DOCNAME).pg $(DOCNAME).pgs $(DOCNAME).tmp $(DOCNAME).toc $(DOCNAME).tp $(DOCNAME).vr
