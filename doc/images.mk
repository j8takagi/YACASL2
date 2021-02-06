INKSCAPE := inkscape
CONVERT := convert

%.png: %.svg
	$(INKSCAPE) -y 0.0 -e $@ $< >$<2png.log 2>&1

%.eps: %.svg
	$(INKSCAPE) --export-type=eps $@ $< >$<2eps.log 2>&1
