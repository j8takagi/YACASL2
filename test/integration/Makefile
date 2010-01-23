TESTGROUPS = `ls | grep "^[^A-Z].*"`
.PHONY: all
all:
	@for target in $(TESTGROUPS); do \
         $(MAKE) -C $$target; \
     done
