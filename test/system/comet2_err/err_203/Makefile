include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/err/err_203.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
