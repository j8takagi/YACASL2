# 無効な-zオプションで、1から10まで加算するopt.caslを実行
include ../CMD.mk               # YACASL2DIR, CASL2
include ../opt_s/CMD.mk         # ASDIR, ASFILE
CASL2FLAG = -z
SETBYOTHER = 0
CMD = $(CASL2) $(CASL2FLAG) $(ASFILE)
include ../TEST.mk
0.txt 1.txt: $(CASL2) $(ASFILE)
	@$(CMD) >>$@ 2>err.txt; \
     if test -s err.txt; then tail -1 err.txt >>$@; else rm -f err.txt; fi
