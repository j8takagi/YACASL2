include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/LD/ld_r_adr__l.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
