.PHONY: texinfo-distclean texinfo-clean texinfo-textmp-clean

CP := cp
DVIPDFMX := dvipdfmx
INSTALL-INFO := install-info
MKDIR := mkdir
RMR := rm -rf
TEX := ptex
TEXI2DVI := TEX=$(TEX) texi2dvi

DVIPDFMX_FLAGS ?=
MAKEINFO_FLAGS ?=
TEXI2DVI_FLAGS ?= -q --texinfo=@afourpaper

%.info: %.texi
	$(MAKEINFO) $(MAKEINFO_FLAGS) -o $@ $<

%.html: %.texi $(CSS)
	$(MAKEINFO) -o $@ --no-split --html --css-include=$(CSS) $<

%_html: %.texi $(CSS)
	if test ! -e $@; then $(MKDIR) $@; fi
	$(CP) $(CSS) $@/
	$(MAKEINFO) -o $@ --html --css-ref=$(CSS) $<

%.pdf: %.dvi
	$(DVIPDFMX) $(DVIPDFMXFLAGS) $<

%.txt: %.texi
	$(MAKEINFO) --no-headers --disable-encoding -o $@ $<

%.xml: %.texi
	@$(MAKEINFO) --docbook -o $@ $<

texinfo-distclean: texinfo-clean
	@$(RMR) *_html *.info *.html *.pdf  *.dvi *.txt

texinfo-clean: texinfo-textmp-clean

texinfo-textmp-clean:
	@$(RM) *.aux *.cp *.cps *.fn *.ky *.log *.pg *.pgs *.tmp *.toc *.tp *.vr
