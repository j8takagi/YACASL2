include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/sample/sum_10.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
