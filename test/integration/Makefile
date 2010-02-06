YACASL2DIR=../..
TESTGROUPS = casl2 comet2 dumpword
.PHONY: check clean
check:
	@for target in $(TESTGROUPS); do $(MAKE) -sC $$target; done
clean:
	@for target in $(TESTGROUPS); do $(MAKE) -sC $$target clean; done
