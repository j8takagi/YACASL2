include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/err/err_202.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	rm -f a.o
