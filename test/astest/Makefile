include List.mk
LOGFILE = test.log
ifdef UNITNAME
  define create
    @mkdir $(UNITNAME)
    @echo "UNITNAME = $(UNITNAME)" >$(UNITNAME)/Makefile; \
     echo "# ASFILE = " >>$(UNITNAME)/Makefile; \
     echo "# CASL2FLAG = " >>$(UNITNAME)/Makefile; \
     echo "# INFILE = " >>$(UNITNAME)/Makefile; \
     echo "include ../Test.mk" >>$(UNITNAME)/Makefile
    @echo $(UNITNAME) \\ >>List.mk
  endef
else
  define create
    @echo "no test created. set UNITNAME"
  endef
endif

.PHONY: all clean logclean testclean check prepare
all: check
check: logclean checkeach report
clean: logclean testclean
logclean:
	@rm -f $(LOGFILE)
testclean:
	@for target in $(TESTS); do \
         $(MAKE) testclean -C $$target; \
     done
checkeach:
	@for target in $(TESTS); do \
         $(MAKE) -C $$target; \
     done
prepare: logclean
	@for target in $(TESTS) ; do \
	     $(MAKE) prepare -C $$target ;\
     done
create:
	$(create)
report:
	@success=`grep "Success" $(LOGFILE) | wc -l`; \
	 all=`cat $(LOGFILE) | wc -l`; \
	 echo "$$success / $$all tests passed. Details in $(PWD)/$(LOGFILE)"; \
	 if test $$success -eq $$all; then \
	   echo "All tests are succeded."; \
	 else \
	   grep "Failure" $(LOGFILE); \
	 fi
