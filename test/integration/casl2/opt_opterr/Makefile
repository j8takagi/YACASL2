# 無効な-zオプションで、1から10まで加算するopt.caslを実行
include ../CMD.mk               # YACASL2DIR, CASL2
include ../opt_s/CMD.mk         # ASDIR, ASFILE
CASL2FLAG = -z
MAINTARGET_OVERRIDE = 1
CMD = $(CASL2) $(CASL2FLAG) $(ASFILE)
include ../TEST.mk
0.txt 1.txt: $(CASL2) $(ASFILE)
	@$(CMD) >>$@ 2>$(ERRFILE); \
     if test -s $(ERRFILE); then tail -1 $(ERRFILE) >>$@; else rm -f $(ERRFILE); fi
