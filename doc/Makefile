DOCNAME = yacasl2
SRC = $(DOCNAME).texi
TEXBIN = /usr/local/teTeX/bin
PTEX = $(TEXBIN)/ptex
TEXINDEX = $(TEXBIN)/texindex
DVI2PDF = $(TEXBIN)/dvipdfmx
TEXOBJDIR = texobj

.PHONY: html pdf clean
$(DOCNAME).html: $(SRC)
	@makeinfo -o $@ --no-split --no-headers --html --css-include=style.css $^
html: $(SRC)
	@cp style.css $@/; \
         makeinfo -o $@ --html --css-ref=style.css $^
$(DOCNAME).dvi: $(SRC)
	@if test ! -s $(TEXOBJDIR); then mkdir $(TEXOBJDIR); fi; \
     cd $(TEXOBJDIR); \
     TEX=ptex texi2dvi --texinfo=@afourpaper -o ../$@ ../$^; \
     cd -
$(DOCNAME).pdf: $(DOCNAME).dvi
	@$(DVI2PDF) $^
clean:
	@rm -rf html $(TEXOBJDIR) $(DOCNAME).html $(DOCNAME).dvi $(DOCNAME).pdf
