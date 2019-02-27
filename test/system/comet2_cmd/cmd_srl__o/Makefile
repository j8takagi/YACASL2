include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/SRL/srl__o.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
