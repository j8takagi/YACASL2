include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/AND/and_r_adr__z.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
