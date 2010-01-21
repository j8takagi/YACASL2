include LIST.mk
LOGFILE = test.log
ifdef UNITNAME
  define create
    @mkdir $(UNITNAME)
    @echo 'include ../CMD.mk    # YACASL2DIR, CASL2' >$(UNITNAME)/Makefile; \
     echo 'UNITNAME = $(UNITNAME)' >>$(UNITNAME)/Makefile; \
     echo 'ASDIR = $$(YACASL2DIR)/as' >>$(UNITNAME)/Makefile; \
     echo 'ASFILE = $$(ASDIR)/$(UNITNAME)' >>$(UNITNAME)/Makefile; \
     echo '# IN = <' >>$(UNITNAME)/Makefile; \
     echo 'CMD = $$(CASL2) $$(CASL2FLAG) $$(ASFILE) $$(IN)' >>$(UNITNAME)/Makefile; \
     echo 'include ../Test.mk' >>$(UNITNAME)/Makefile
    @echo $(UNITNAME) \\ >>List.mk
  endef
else
  define create
    @echo "no test created. set UNITNAME"
  endef
endif

.PHONY: all check cleanall clean prepare create report
all: clean check report
clean:
	@for target in $(TESTS); do \
         $(MAKE) clean -C $$target; \
     done
	@rm -f $(LOGFILE)
check:
	@for target in $(TESTS); do \
         $(MAKE) check -C $$target; \
     done
report: $(LOGFILE)
	@success=`grep "Success" $(LOGFILE) | wc -l`; \
     all=`cat $(LOGFILE) | wc -l`; \
     echo "$$success / $$all tests passed. Details in `pwd`/$(LOGFILE)"; \
     if test $$success -eq $$all; then \
       echo "All tests are succeded."; \
     else \
       grep "Failure" $(LOGFILE); \
     fi
prepare: cleanall
	@for target in $(TESTS) ; do \
         $(MAKE) prepare -C $$target ;\
     done
cleanall:
	@for target in $(TESTS); do \
         $(MAKE) cleanall -C $$target; \
     done
	@rm -f $(LOGFILE)
create:
	$(create)
$(LOGFILE):
	@for target in $(TESTS); do \
         cat <$$target >>$(LOGFILE); \
     done
