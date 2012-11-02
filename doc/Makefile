MANUAL = manual
INSTALLDOC = install
CASL2SPEC = casl2_spec
MANUAL_SYSINFO = yacasl2.info
CSS = style.css

MV ?= mv
CP ?= cp
MKDIR ?= mkdir
RMF = rm -f
RMRF ?= rm -rf
PTEX ?= ptex
TEXI2DVI ?= texi2dvi
DVI2PDF ?= dvipdfmx
MAKEINFO ?= gmakeinfo
INSTALL ?= install
INSTALL-INFO ?= ginstall-info
GZIP = gzip

prefix ?= ~
infodir ?= $(prefix)/share/info

.PHONY: all info html htmls pdf install-info uninstall-info clean clean-textmp

.INTERMEDIATE: *.dvi

base: ../INSTALL $(MANUAL_SYSINFO) $(MANUAL).html

all: info html htmls pdf ../INSTALL $(MANUAL_SYSINFO)

info: $(MANUAL_SYSINFO) $(INSTALLDOC).info

html: $(MANUAL).html $(INSTALLDOC).html

htmls: $(MANUAL)_html

pdf: $(INSTALLDOC).pdf $(MANUAL).pdf

$(MANUAL_SYSINFO): $(MANUAL).texi
	$(MAKEINFO) $<

%.html: %.texi $(CSS)
	$(MAKEINFO) -o $@ --no-split --html --css-include=$(CSS) $<

%_html: %.texi $(CSS)
	$(INSTALL) -d $@
	$(INSTALL) $(CSS) $@/
	$(MAKEINFO) -o $@ --html --css-ref=$(CSS) $<

%.pdf: %.dvi
	$(DVI2PDF) $<

%.dvi: %.texi
	TEX=$(PTEX) $(TEXI2DVI) -q --texinfo=@afourpaper -o $@ $<
	@$(MAKE) clean-textmp

%.txt: %.texi
	$(MAKEINFO) --no-headers --disable-encoding -o $@ $<

install-info: $(MANUAL_SYSINFO)
	$(INSTALL) -d $(infodir)
	$(INSTALL) $(MANUAL_SYSINFO) $(infodir)/
	$(INSTALL-INFO) $(infodir)/$(MANUAL_SYSINFO) $(infodir)/dir
	$(GZIP) -f $(infodir)/$(MANUAL_SYSINFO)

uninstall-info: $(MANUAL).info
	$(INSTALL-INFO) --delete $(infodir)/$(MANUAL_SYSINFO) $(infodir)/dir
	$(RMF) $(infodir)/$(MANUAL_SYSINFO)*

$(INSTALLDOC).html: $(INSTALLDOC).texi $(CSS)
	$(MAKEINFO) -o $@ --no-split --html --css-include=$(CSS) $<

../INSTALL: $(INSTALLDOC).txt
	$(CP) $< $@

clean: clean-textmp
	@$(RMRF) *_html *.info *.html *.pdf  *.dvi *.txt

clean-textmp:
	@$(RMF) *.aux *.cp *.cps *.fn *.ky *.log *.pg *.pgs *.tmp *.toc *.tp *.vr
