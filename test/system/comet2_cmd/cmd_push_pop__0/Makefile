include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/POP/push_pop__0.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
