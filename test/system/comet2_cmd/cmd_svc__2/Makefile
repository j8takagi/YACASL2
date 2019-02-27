include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/SVC/svc__2.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
