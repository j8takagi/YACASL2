include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/SUBA/suba_r_adr__ao.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
