RSVG_CONVERT := rsvg-convert
GS := gs
GS_EPSCROP := $(GS) -q -dNOPAUSE -dBATCH -sDEVICE=eps2write -dEPSCrop

%.png: %.svg
	$(RSVG_CONVERT) --format=png --output=$@ $< >$<2png.log 2>&1

%.eps: %.svg
	$(RSVG_CONVERT) --format=eps --output=$@.tmp $< >$<2eps.log 2>&1 && $(GS_EPSCROP) -sOutputFile=$@ $@.tmp

%.pdf: %.svg
	$(RSVG_CONVERT) --format=pdf --output=$@ $< >$<2pdf.log 2>&1
