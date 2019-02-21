include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/ST/st_r_adr.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
