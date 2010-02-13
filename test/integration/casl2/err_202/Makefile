include ../CMD.mk    # YACASL2DIR, CASL2
ASDIR = $(YACASL2DIR)/as
CASL2LIBDIR = $(YACASL2DIR)/as/casl2lib
ASFILE = $(YACASL2DIR)/as/err/err_202.casl
CASL2FLAG = -M32
INFILE = $(YACASL2DIR)/as/err/teststr.txt
CMD = $(CASL2) $(CASL2FLAG) $(ASFILE) <$(INFILE)
include ../TEST.mk
