YACASL2DIR=../../../..
UNITNAME = `pwd | xargs basename`
OBJFILE = a.o
CASL2 = $(YACASL2DIR)/casl2
COMET2 = $(YACASL2DIR)/comet2
CMD = $(COMET2) $(COMET2FLAG) $(OBJFILE)
