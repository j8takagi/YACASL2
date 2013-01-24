.PHONY: clean systemtest-clean unittest-clean

clean: systemtest-clean unittest-clean

systemtest-clean:
	@$(MAKE) -sC system/casl2 clean
	@$(MAKE) -sC system/comet2 clean
	@$(MAKE) -sC system/dumpword clean

unittest-clean:
	@$(MAKE) -sC unit clean
