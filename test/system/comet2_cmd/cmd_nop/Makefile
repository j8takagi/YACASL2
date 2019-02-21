include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/NOP/nop.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
