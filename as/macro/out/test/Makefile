YACASL2DIR=../../../..
CASL2 = $(YACASL2DIR)/casl2

.PHPNY: check clean
check: clean report.txt
code_out_macro.txt: out_macro.casl
	@$(CASL2) -O $^ && od -t x2 a.o >$@
code_out_cmds.txt: out_cmds.casl
	@$(CASL2) -O $^ && od -t x2 a.o >$@
diff.txt: code_out_macro.txt code_out_cmds.txt
	@-diff -c $^ >$@ 2>&1
report.txt: diff.txt
	@echo -n "Test " >$@; \
     if test ! -s $^; then echo -n "Success " >>$@; rm -f $^; else echo -n "Failure " >>$@; fi; \
     echo `date +"%F %T"` >>$@
clean:
	@rm -f code_out_macro.txt code_out_cmds.txt diff.txt report.txt a.o
