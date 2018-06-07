all: $(foreach ext,.png .eps,$(addsuffix $(ext),$(basename $(wildcard *.svg))))

clean:
	$(RM) *.log

distclean: clean
	$(RM) *.png *.eps *.jpg

include ../../../images.mk
