YACASL2DIR= ../../../..
CASL2 = $(YACASL2DIR)/casl2
CASL2FLAG = -M640
LIBDIR = $(YACASL2DIR)/as/casl2lib
ASFILE = inl.casl $(LIBDIR)/inl.casl $(LIBDIR)/outl.casl
IN = teststr.txt
CMD = $(CASL2) $(CASL2FLAG) $(ASFILE) <$(IN)
include $(YACASL2DIR)/test/TEST.mk
