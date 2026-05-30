SED := sed
DATE := date

DOCS := install manual
CSS := style.css

prefix ?= ~
infodir ?= $(prefix)/share/info

TEXITARGETS := install.html manual.html

INSTALL-INFO-TARGETS := yacasl2.info

YEAR := $(shell $(DATE) '+%Y')

.PHONY: base all info html htmls pdf casl2_spec distclean clean

base: copyright install.txt info html

all: base htmls pdf casl2_spec

copyright:
	$(SED) -i.sed_bak 's/Copyright @copyright{} 2010-20[0-9][0-9]/Copyright @copyright{} 2010-$(YEAR)/g' *.texi && $(RM) *.sed_bak

info: yacasl2.info

html: $(addsuffix .html,$(DOCS))

htmls: $(addsuffix _html,$(DOCS))

$(addsuffix .html,$(DOCS)): style.css

$(addsuffix _html,$(DOCS)): style.css

pdf: $(addsuffix .pdf,$(DOCS))

casl2_spec:
	$(MAKE) -C casl2_spec

yacasl2.info: manual.texi
	$(MAKEINFO) $(MAKEINFO_FLAGS) $<

# makeで作成されたドキュメントはすべて削除
distclean: clean texinfo-distclean
	$(RM) install.txt *.info *.html *_html *.pdf *.sed_bak
	$(MAKE) -C casl2_spec distclean

# make baseで作成されたドキュメントは残す
clean: texinfo-clean
	$(RM) *.sed_bak
	$(MAKE) -C casl2_spec clean

include texinfo.mk

include install-info.mk
