DOCNAME = yacasl2
SRC = $(DOCNAME).texi
CSS = style.css

PTEX = ptex
TEXI2DVI = texi2dvi
DVI2PDF = dvipdfmx
MAKEINFO = makeinfo

.PHONY: all htmlnosplit pdf clean

.INTERMEDIATE: $(DOCNAME).dvi

doc: info htmlnosplit html pdf

htmlnosplit: $(DOCNAME).html

html: $(SRC) $(CSS)
	@if test ! -s $@; then mkdir $@; fi;
	@cp $(CSS) $@/$(CSS)
	@$(MAKEINFO) -o $@ --html --css-ref=style.css $(SRC)

pdf: $(DOCNAME).pdf

info: $(DOCNAME).info

$(DOCNAME).info: $(SRC)
	@$(MAKEINFO) -o $@ $^

$(DOCNAME).html: $(SRC) $(CSS)
	@$(MAKEINFO) -o $@ --no-split --html --css-include=$(CSS) $(SRC)

$(DOCNAME).pdf: $(DOCNAME).dvi
	@$(DVI2PDF) $^

$(DOCNAME).dvi: $(SRC)
	@TEX=$(PTEX) $(TEXI2DVI) -q --texinfo=@afourpaper -o $@ $<
	@rm -rf $(DOCNAME).aux $(DOCNAME).cp $(DOCNAME).cps $(DOCNAME).fn $(DOCNAME).ky $(DOCNAME).log $(DOCNAME).pg $(DOCNAME).pgs $(DOCNAME).tmp $(DOCNAME).toc $(DOCNAME).tp $(DOCNAME).vr

clean:
	@rm -rf html $(DOCNAME).info $(DOCNAME).pdf $(DOCNAME).html $(DOCNAME).dvi
	@rm -rf $(DOCNAME).aux $(DOCNAME).cp $(DOCNAME).cps $(DOCNAME).fn $(DOCNAME).ky $(DOCNAME).log $(DOCNAME).pg $(DOCNAME).pgs $(DOCNAME).tmp $(DOCNAME).toc $(DOCNAME).tp $(DOCNAME).vr
