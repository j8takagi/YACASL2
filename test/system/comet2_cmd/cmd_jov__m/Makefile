include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/JOV/jov__m.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
