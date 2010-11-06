# 無効な-zオプションで、1から10まで加算するopt.caslを実行
include ../CMD.mk               # YACASL2DIR, CASL2
include ../opt_s/CMD.mk         # ASDIR, ASFILE
CASL2FLAG = -z
MAINTARGET_OVERRIDE = 1
CMD = $(CASL2) $(CASL2FLAG) $(ASFILE)
include ../TEST.mk
0.txt 1.txt:
	@$(CMD) >>$@ 2>$(ERR_FILE); \
     if test -s $(ERR_FILE); then tail -1 $(ERR_FILE) >>$@; else rm -f $(ERR_FILE); fi
