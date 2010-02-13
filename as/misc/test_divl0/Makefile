YACASL2DIR= ../../../../..
CASL2 = $(YACASL2DIR)/casl2
CASL2LIBDIR = $(YACASL2DIR)/as/casl2lib
ASFILE = test_ptn4.casl ../ptn4.casl ../ptn1.casl ../ptn2.casl $(CASL2LIBDIR)/outb.casl
CMD = $(CASL2) $(ASFILE)
include $(YACASL2DIR)/test/TEST.mk

include ../CMD.mk    # YACASL2DIR, CASL2
ASDIR = $(YACASL2DIR)/as/FUNC
ASFILE = test_divl.casl $(ASDIR)/divl0.casl $(ASDIR)/outl_main.casl $(ASDIR)/rev.casl
CMD = $(CASL2) $(ASFILE)
include ../TEST.mk
