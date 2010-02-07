include ../CMD.mk    # YACASL2DIR, CASL2
LIBDIR = $(YACASL2DIR)/as/casl2lib
ASDIR = $(YACASL2DIR)/as/casl2lib_driver
ASFILE = $(ASDIR)/inl.casl $(LIBDIR)/inl.casl $(LIBDIR)/outl.casl
IN = teststr.txt
CMD = $(CASL2) $(ASFILE) <$(IN)
include ../TEST.mk
