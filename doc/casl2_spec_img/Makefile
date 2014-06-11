INKSCAPE := inkscape -z
CONVERT := convert

all: $(foreach ext,.png .eps,$(addsuffix $(ext),$(basename $(wildcard *.svg))))

%.png: %.svg
	$(INKSCAPE) -y 0.0 -e $@ $< >$<2png.log 2>&1

%.eps: %.svg
	$(INKSCAPE) -E $@ $< >$<2eps.log 2>&1

%.jpg: %.png
	$(CONVERT) $< $@

clean:
	$(RM) *.log

distclean: clean
	$(RM) *.png *.eps *.jpg
