include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/ADDA/adda_r1_r2.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
