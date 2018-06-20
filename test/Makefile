.PHONY: systemtest clean system-clean unit-clean

systemtest:
	@$(MAKE) -sC system check

clean: systemtest-clean unittest-clean

system-clean:
	@$(MAKE) -sC system clean

unit-clean:
	@$(MAKE) -sC unit clean
