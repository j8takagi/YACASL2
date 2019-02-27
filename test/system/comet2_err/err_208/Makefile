include ../Define.mk
include ../Test.mk

cmd: a.o teststr.txt

a.o: ../../../../as/err/err_208.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
