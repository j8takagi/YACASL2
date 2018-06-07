VPATH := img

CSS := ../style.css

TEXITARGETS := fe_2018_04.html fe_2018_04.pdf

.PHONY: all clean distclean

all: $(TEXITARGETS)

clean: texinfo-clean
	$(RM) *.bak
	$(MAKE) -C img clean

distclean: clean texinfo-distclean
	$(RM) *.d *.html *.pdf
	$(MAKE) -C img distclean

include ../texinfo_exam.mk

include ../../images.mk
