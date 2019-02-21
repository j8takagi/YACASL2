include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/SLA/sla__oz.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
