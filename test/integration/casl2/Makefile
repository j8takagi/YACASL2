# テストグループ
# make         : すべてのテストを実施し、ログを作成
# make all     : ↓
# make clean   : すべてのテストで、「make」で生成されたファイルをクリア
# make check   : すべてのテストを実施
# make cleanall: すべてのテストで、「make」と「make prepare」で生成されたファイルをクリア
# make prepare : すべてのテストの、想定結果を出力
# make create  : UNITNAMEで指定されたテストを新規に作成
GROUPNAME = `pwd | xargs basename`
TESTS = `ls | grep "^[^A-Z].*"`
LOGFILE = Test.log

.PHONY: all check checkeach report clean cleanall prepare create
check: checkeach report
checkeach:
	@rm -f $(LOGFILE)
	@for target in $(TESTS); do $(MAKE) check -C $$target; done
$(LOGFILE):
	@for target in $(TESTS); do \
         cat <$$target/report.txt >>$(LOGFILE) || echo $$target ": no report" >>$(LOGFILE); \
     done
report: $(LOGFILE)
	@success=`grep "Success" $(LOGFILE) | wc -l`; \
     all=`cat $(LOGFILE) | wc -l`; \
     echo "$(GROUPNAME): $$success / $$all tests passed. Details in `pwd`/$(LOGFILE)"; \
     if test $$success -eq $$all; then \
       echo "$(GROUPNAME): All tests are succeded."; \
     fi
clean:
	@for target in $(TESTS); do $(MAKE) clean -C $$target; done
	@rm -f $(LOGFILE)
cleanall:
	@for target in $(TESTS); do $(MAKE) cleanall -C $$target; done
	@rm -f $(LOGFILE)
prepare:
	@for target in $(TESTS) ; do $(MAKE) prepare -C $$target ; done
create:
ifndef UNITNAME
	@echo "no test created. set UNITNAME"
else
	@mkdir $(UNITNAME)
	@echo 'include ../CMD.mk    # YACASL2DIR, CASL2' >$(UNITNAME)/Makefile
ifdef CASL2FLAG
	@echo 'CASL2FLAG = $(CASL2FLAG)' >>$(UNITNAME)/Makefile
endif
	@echo 'ASDIR = $$(YACASL2DIR)/as' >>$(UNITNAME)/Makefile
	@echo 'CASL2LIBDIR = $$(YACASL2DIR)/as/casl2lib' >>$(UNITNAME)/Makefile
ifdef ASFILE
	@echo 'ASFILE = $(ASFILE)' >>$(UNITNAME)/Makefile
endif
ifdef INFILE
	@echo 'INFILE = $$(INFILE)' >>$(UNITNAME)/Makefile
endif
	@echo -n 'CMD = $$(CASL2) $$(CASL2FLAG) $$(ASFILE)' >>$(UNITNAME)/Makefile
ifdef INFILE
	@echo '< $$(INFILE)' >>$(UNITNAME)/Makefile
endif
	@echo >>$(UNITNAME)/Makefile; \
     echo 'include ../TEST.mk' >>$(UNITNAME)/Makefile
endif
