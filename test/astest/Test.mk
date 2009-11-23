CASL2 = ../../../casl2
ifndef CASL2FLAG
	CASL2FLAG = -tals
endif
COMET2 = ../../../comet2
ifndef COMET2FLAG
	COMET2FLAG = -td
endif
ifndef ASFILE
	ASFILE=../../../as/$(UNITNAME).casl
endif
ifdef INFILE
	IN= <$(INFILE)
endif
LOGFILE = ../test.log
define report
  @echo -n "$(UNITNAME): Test " >$@
  @if test ! -s $^; then \
    echo -n "Success " >>$@; \
  else \
    echo -n "Failure " >>$@; \
  fi
  @echo `date +"%F %T"` >>$@
endef

.PHPNY: all check ocheck prepare oprepare testclean otestclean orgclean oorgclean
check: testclean logadd
ocheck: otestclean ologadd
all: check ocheck
prepare: orgclean testclean 0.txt
oprepare: oorgclean otestclean 0.o 0.o.txt
orgclean:
	@rm -f 0.txt
oorgclean:
	@rm -f 0.o 0.o.txt
testclean:
	@rm -f 1.txt diff.txt report.txt
otestclean:
	@rm -f 1.o 1.o.txt diff.o.txt report.o.txt
0.txt 1.txt: $(CASL2) $(ASFILE) $(INFILE)
	@-$(CASL2) $(CASL2FLAG) $(ASFILE) $(IN) >res.txt 2>err.txt; \
      cat res.txt err.txt >$@; \
      rm res.txt err.txt
diff.txt: 1.txt
	@-diff 0.txt 1.txt >$@ 2>&1
report.txt: diff.txt
	$(report)
logadd: report.txt
	@cat $^ >>$(LOGFILE)
0.o 1.o: $(CASL2) $(ASFILE)
	@-$(CASL2) -o $@ $(ASFILE)
0.o.txt: $(COMET2) 0.o
	@-$(COMET2) $(COMET2FLAG) 0.o >0.o.txt
1.o.txt: $(COMET2) 1.o
	@-$(COMET2) $(COMET2FLAG) 1.o >1.o.txt
diff.o.txt: 1.o.txt
	@-diff 0.o.txt 1.o.txt >>$@ 2>&1
report.o.txt: diff.o.txt
	$(report)
ologadd: report.o.txt
	@cat $^ >>$(LOGFILE)
