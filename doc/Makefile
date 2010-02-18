DOCNAME = yacasl2
SRC = $(DOCNAME).texi
CSS = style.css
TEXBIN = /usr/local/teTeX/bin
PTEX = $(TEXBIN)/ptex
TEXINDEX = $(TEXBIN)/texindex
DVI2PDF = $(TEXBIN)/dvipdfmx
TEXOBJDIR = texobj

.PHONY: all htmlnosplit pdf clean
all: htmlnosplit html pdf
htmlnosplit: $(DOCNAME).html
$(DOCNAME).html: $(SRC) $(CSS)
	@makeinfo -o $@ --no-split --no-headers --html --css-include=$(CSS) $(SRC)
html: $(SRC) $(CSS)
	@if test ! -s $@; then mkdir $@; fi; \
     cp $(CSS) $@/$(CSS); \
     makeinfo -o $@ --html --css-ref=style.css $(SRC)
pdf: $(DOCNAME).pdf
$(DOCNAME).pdf: $(DOCNAME).dvi
	@$(DVI2PDF) $^
$(DOCNAME).dvi: $(SRC)
	@if test ! -s $(TEXOBJDIR); then mkdir $(TEXOBJDIR); fi; \
     cd $(TEXOBJDIR); \
     TEX=ptex texi2dvi --texinfo=@afourpaper -o ../$@ ../$^; \
     cd -
clean:
	@rm -rf html $(TEXOBJDIR) $(DOCNAME).html $(DOCNAME).dvi $(DOCNAME).pdf
