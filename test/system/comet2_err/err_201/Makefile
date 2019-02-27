include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/err/err_201.casl
	../../../../casl2 -O $<

cleanobj:
	rm -f a.o
