RMR := $(RM) -r
SED := sed

DOCS := install manual casl2_spec
CSS := style.css

prefix ?= ~
infodir ?= $(prefix)/share/info

.PHONY: base all info html htmls pdf distclean clean

base: install.txt info html

all: base html pdf

info: yacasl2.info

html: $(addsuffix .html,$(DOCS))

htmls: $(addsuffix _html,$(DOCS))

pdf: $(addsuffix .pdf,$(DOCS))

yacasl2.info: manual.texi
	$(MAKEINFO) $(MAKEINFO_FLAGS) $<

# makeで作成されたドキュメントはすべて削除
distclean: clean texinfo-distclean
	$(RM) install.txt *.info *.html *_html *.pdf

# make baseで作成されたドキュメントは残す
clean: texinfo-clean
	$(RM) *.bak

include texinfo.mk

include install-info.mk
