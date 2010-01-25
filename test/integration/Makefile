YACASL2DIR=../..
TESTGROUPS = casl2 comet2 dumpword
.PHONY: all clean
all:
	@for target in $(TESTGROUPS); do $(MAKE) -sC $$target; done
clean:
	@for target in $(TESTGROUPS); do $(MAKE) -sC $$target clean; done
