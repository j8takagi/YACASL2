include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/CPL/cpl_r_adr__ls.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
