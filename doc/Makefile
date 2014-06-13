RMR := $(RM) -r
SED := sed

DOCS := install manual
CSS := style.css

prefix ?= ~
infodir ?= $(prefix)/share/info

INSTALL-INFO-TARGETS := yacasl2.info

.PHONY: base all info html htmls pdf casl2_spec distclean clean

base: install.txt info html

all: base htmls pdf casl2_spec

info: yacasl2.info

html: $(addsuffix .html,$(DOCS))

htmls: $(addsuffix _html,$(DOCS))

pdf: $(addsuffix .pdf,$(DOCS))

casl2_spec:
	$(MAKE) -C casl2_spec

yacasl2.info: manual.texi
	$(MAKEINFO) $(MAKEINFO_FLAGS) $<

# makeで作成されたドキュメントはすべて削除
distclean: clean texinfo-distclean
	$(RM) install.txt *.info *.html *_html *.pdf
	$(MAKE) -C casl2_spec distclean

# make baseで作成されたドキュメントは残す
clean: texinfo-clean
	$(RM) *.bak
	$(MAKE) -C casl2_spec clean

include texinfo.mk

include install-info.mk
