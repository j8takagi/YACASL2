DOCS := install manual

CSS := style.css

CP := cp
DVIPDFMX := dvipdfmx
GZIP := gzip
INSTALL := install
INSTALL-INFO := install-info
MKDIR := mkdir
RMR := rm -rf
TEX := ptex
TEXI2DVI := TEX=ptex texi2dvi
TEX2DVI_FLAGS = -q --texinfo=@afourpaper

prefix ?= ~
infodir ?= $(prefix)/share/info

.PHONY: base all info html htmls pdf install-info uninstall-info clean textmp-clean

.INTERMEDIATE: *.dvi

base: install.txt info html

all: base htmls pdf

info: yacasl2.info

html: $(addsuffix .html,$(DOCS))

htmls: $(addsuffix _html,$(DOCS))

pdf: $(addsuffix .pdf,$(DOCS))

yacasl2.info: manual.texi
	$(MAKEINFO) $(MAKEINFO_FLAGS) $<

$(addsuffix .html,$(DOCS)) $(addsuffix _html,$(DOCS)): $(CSS)

%.html: %.texi
	$(MAKEINFO) -o $@ --no-split --html --css-include=$(CSS) $<

%_html: %.texi
	if test ! -e $@; then $(MKDIR) $@; fi
	$(CP) $(CSS) $@/
	$(MAKEINFO) -o $@ --html --css-ref=$(CSS) $<

%.pdf: %.dvi
	$(DVIPDFMX) $<

%.txt: %.texi
	$(MAKEINFO) --no-headers --disable-encoding -o $@ $<

install-info: yacasl2.info
	$(INSTALL) -d $(infodir)
	$(INSTALL) $< $(infodir)/
	$(INSTALL-INFO) $(infodir)/$< $(infodir)/dir
	$(GZIP) -f $(infodir)/$<

uninstall-info:
	$(INSTALL-INFO) --delete $(infodir)/yacasl2.info $(infodir)/dir
	$(RM) $(infodir)/yacasl2.info

clean: textmp-clean
	@$(RMR) *_html *.info *.html *.pdf  *.dvi *.txt

textmp-clean:
	@$(RM) *.aux *.cp *.cps *.fn *.ky *.log *.pg *.pgs *.tmp *.toc *.tp *.vr
