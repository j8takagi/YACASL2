DOCNAME = yacasl2
SRC = $(DOCNAME).texi
CSS = style.css
TEXBIN = /usr/local/texlive/p2009/bin/i386-apple-darwin10.3.0
PTEX = $(TEXBIN)/ptex
TEXINDEX = $(TEXBIN)/texindex
DVI2PDF = $(TEXBIN)/dvipdfmx
MAKEINFO = makeinfo

.PHONY: all htmlnosplit pdf clean

.INTERMEDIATE: yacasl2.dvi

htmlnosplit: $(DOCNAME).html

all: info htmlnosplit html pdf

html: $(SRC) $(CSS)
	@if test ! -s $@; then mkdir $@; fi;
	@cp $(CSS) $@/$(CSS)
	@$(MAKEINFO) -o $@ --html --css-ref=style.css $(SRC)

pdf: $(DOCNAME).pdf

info: $(DOCNAME).info

$(DOCNAME).info: $(SRC)
	@$(MAKEINFO) -o $@ $^

$(DOCNAME).html: $(SRC) $(CSS)
	@$(MAKEINFO) -o $@ --no-split --no-headers --html --css-include=$(CSS) $(SRC)

$(DOCNAME).pdf: $(DOCNAME).dvi
	@$(DVI2PDF) $^

$(DOCNAME).dvi: $(SRC)
	@TEX=ptex texi2dvi -q --texinfo=@afourpaper -o $@ $<
	@rm -rf $(DOCNAME).aux $(DOCNAME).cp $(DOCNAME).cps $(DOCNAME).fn $(DOCNAME).ky $(DOCNAME).log $(DOCNAME).pg $(DOCNAME).pgs $(DOCNAME).tmp $(DOCNAME).toc $(DOCNAME).tp $(DOCNAME).vr

clean:
	@rm -rf html $(DOCNAME).info $(DOCNAME).pdf $(DOCNAME).html $(DOCNAME).dvi
