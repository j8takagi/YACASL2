include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/LAD/lad_r_adr_x__o.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
