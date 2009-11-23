include List.mk
LOGFILE = test.log
ifdef UNITNAME
  define create
    @mkdir $(UNITNAME)
    @echo "UNITNAME = $(UNITNAME)" >$(UNITNAME)/Makefile; \
     echo "UCLASS = $(UCLASS)" >>$(UNITNAME)/Makefile; \
     echo "include ../Test.mk" >>$(UNITNAME)/Makefile
    @echo "$(UNITNAME) \\" >>List.mk;
    @cp Template.c $(UNITNAME)/$(UNITNAME).c;
  endef
else
  define create
    @echo "no test created. set UNITNAME, UCLASS={ AS | EXEC | COMMON }"
  endef
endif

.PHONY: all logclean testclean check prepare
all: logclean check clean report
clean: logclean testclean
logclean:
	@rm -f $(LOGFILE)
testclean:
	@for target in $(TESTS); do \
         $(MAKE) testclean -C $$target; \
     done
check: $(LOGFILE)
$(LOGFILE):
	@for target in $(TESTS); do \
         $(MAKE) -C $$target; \
     done
prepare:
	@for target in $(TESTS) ; do \
	     $(MAKE) prepare -C $$target ;\
     done
create:
	$(create)
report: $(LOGFILE)
	@success=`grep "Success" $(LOGFILE) | wc -l`; \
	 all=`cat $(LOGFILE) | wc -l`; \
	 echo "$$success / $$all tests passed."; \
	 if test $$success -eq $$all; then \
	   echo "All Tests are passed."; \
	 else \
	   grep "Failure" $(LOGFILE); \
	 fi
