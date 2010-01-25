YACASL2DIR=../..
TARGETS = casl2 comet2 dumpword
.PHONY: all
all:$(TARGETS)
%: $(YACASL2DIR)/%
	$(MAKE) -C $@
