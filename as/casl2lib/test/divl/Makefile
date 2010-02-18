YACASL2DIR= ../../../..
CASL2 = $(YACASL2DIR)/casl2
CASL2FLAG = -M640
LIBDIR = $(YACASL2DIR)/as/casl2lib
ASFILE = divl.casl $(LIBDIR)/outl.casl    # DIVLは、outl.caslに含まれる
CMD = $(CASL2) $(CASL2FLAG) $(ASFILE)
include $(YACASL2DIR)/test/integration/casl2/TEST.mk
