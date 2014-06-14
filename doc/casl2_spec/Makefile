VPATH := img

RMR := $(RM) -r
SED := sed

CSS := style.css

TEXITARGETS := casl2_spec.html

.PHONY: all clean distclean

all: casl2_spec.html casl2_spec.pdf

clean: texinfo-clean
	$(RM) *.bak
	$(MAKE) -C img clean

distclean: clean texinfo-distclean
	$(RM) *.d *.html *.pdf
	$(MAKE) -C img distclean

#include casl2_spec.dep

include texinfo.mk

include images.mk
