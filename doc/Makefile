TEXBIN = /usr/local/teTeX/bin
PTEX = $(TEXBIN)/ptex
TEXINDEX = $(TEXBIN)/texindex
DVI2PDF = $(TEXBIN)/dvipdfmx
TEMPPDF = temp_pdf

.PHPNY: html pdf clean
html: yacasl2.texi
	@makeinfo --html --css-ref=style.css $^
pdf: yacasl2.texi
	@if test ! -s $(TEMPPDF); then mkdir $(TEMPPDF); fi; \
     cd $(TEMPPDF); \
     $(PTEX) ../$^; \
     $(TEXINDEX) yacasl2.??; \
     $(DVI2PDF) yacasl2.dvi; \
     mv yacasl2.pdf ..; \
     cd -
clean:
	@rm -rf yacasl2/index.html $(TEMPPDF)
