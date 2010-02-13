include ../CMD.mk    # YACASL2DIR, CASL2
CASL2FLAG = -atd -M64
ASDIR = $(YACASL2DIR)/as
CASL2LIBDIR = $(YACASL2DIR)/as/casl2lib
ASFILE = $(YACASL2DIR)/as/cmd/SVC/svc1.casl
INFILE = teststr.txt
CMD = $(CASL2) $(CASL2FLAG) $(ASFILE) <$(INFILE)
include ../TEST.mk
