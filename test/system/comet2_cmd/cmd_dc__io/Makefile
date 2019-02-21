include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/DC/dc__io.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
