DOCNAME = yacasl2
SRC = $(DOCNAME).texi
CSS = style.css

MV = mv
CP = cp
MKDIR = mkdir
RMRF = rm -rf
PTEX = ptex
TEXI2DVI = texi2dvi
DVI2PDF = dvipdfmx
MAKEINFO = makeinfo

.PHONY: doc info htmlnosplit html pdf clean clean_docs clean_tex_tmp

.INTERMEDIATE: $(DOCNAME).dvi

doc: info htmlnosplit html pdf

info: $(DOCNAME).info

htmlnosplit: index.html

index.html: $(SRC) $(CSS)
	$(MAKEINFO) -o $@ --no-split --html --css-include=$(CSS) $(SRC)

html: $(SRC) $(CSS)
	if test ! -e $@; then $(MKDIR) $@; fi;
	$(CP) $(CSS) $@/$(CSS)
	$(MAKEINFO) -o $@ --html --css-ref=$(CSS) $(SRC)

pdf: $(DOCNAME).pdf

$(DOCNAME).pdf: $(DOCNAME).dvi
	$(DVI2PDF) $^

$(DOCNAME).dvi: $(SRC)
	TEX=$(PTEX) $(TEXI2DVI) -q --texinfo=@afourpaper -o $@ $<
	@$(MAKE) clean_tex_tmp

clean: clean_docs clean_tex_tmp

clean_docs:
	@$(RMRF) html $(DOCNAME).info $(DOCNAME).pdf $(DOCNAME).html

clean_tex_tmp:
	@$(RMRF) $(DOCNAME).aux $(DOCNAME).cp $(DOCNAME).cps $(DOCNAME).fn $(DOCNAME).ky $(DOCNAME).log $(DOCNAME).pg $(DOCNAME).pgs $(DOCNAME).tmp $(DOCNAME).toc $(DOCNAME).tp $(DOCNAME).vr
