# テストグループ
# make         : すべてのテストを実施し、ログを作成
# make all     : ↓
# make clean   : すべてのテストで、「make」で生成されたファイルをクリア
# make check   : すべてのテストを実施
# make cleanall: すべてのテストで、「make」と「make prepare」で生成されたファイルをクリア
# make prepare : すべてのテストの、想定結果を出力
# make create  : UNITNAMEで指定されたテストを新規に作成
TESTS = `ls | grep "^[^A-Z].*"`
LOGFILE = TEST.log

.PHONY: all check report clean cleanall prepare create
all: check report
check:
	@rm -f $(LOGFILE)
	@for target in $(TESTS); do \
         $(MAKE) check -C $$target; \
     done
$(LOGFILE):
	@for target in $(TESTS); do \
         cat <$$target/report.txt >>$(LOGFILE); \
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
clean:
	@for target in $(TESTS); do $(MAKE) clean -C $$target; done
	@rm -f $(LOGFILE)
cleanall:
	@for target in $(TESTS); do $(MAKE) cleanall -C $$target; done
	@rm -f $(LOGFILE)
prepare:
	@for target in $(TESTS) ; do $(MAKE) prepare -C $$target ; done
create:
ifdef UNITNAME
	@mkdir $(UNITNAME)
	@echo 'CMD = ' >>$(UNITNAME)/Makefile
else
	@echo "no test created. set UNITNAME"
endif
