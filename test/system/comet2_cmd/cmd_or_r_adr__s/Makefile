include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/OR/or_r_adr__s.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
