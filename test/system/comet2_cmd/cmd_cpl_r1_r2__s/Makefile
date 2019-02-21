include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/CPL/cpl_r1_r2__s.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
