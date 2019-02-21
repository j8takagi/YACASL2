include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/SRA/sra__o.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
