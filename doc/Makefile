DOCS := install manual
CSS := style.css

prefix ?= ~
infodir ?= $(prefix)/share/info

.PHONY: base all info html htmls pdf distclean clean

base: install.txt info html

all: base htmls pdf

info: yacasl2.info

html: $(addsuffix .html,$(DOCS))

htmls: $(addsuffix _html,$(DOCS))

pdf: $(addsuffix .pdf,$(DOCS))

yacasl2.info: manual.texi
	$(MAKEINFO) $(MAKEINFO_FLAGS) $<

distclean: texinfo-distclean

clean: texinfo-clean

include texinfo.mk

include install-info.mk
