RMR := $(RM) -r
SED := sed

DOCS := install manual
CSS := style.css

prefix ?= ~
infodir ?= $(prefix)/share/info

.PHONY: base all info html htmls pdf distclean clean

base: install.txt info html

all: base htmls pdf

info: yacasl2.info

html: $(addsuffix .html,$(DOCS))
	for f in $^; do $(SED) -i.bak -e 's!dir\.html#Top!\.\./!g' $$f && $(RM) $$f.bak; done

htmls: $(addsuffix _html,$(DOCS))
	for d in $^; do $(SED) -i.bak -e 's!\.\./dir/index\.html!\.\./\.\./!g' $$d/index.html && $(RM) $$d/index.html.bak; done

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
