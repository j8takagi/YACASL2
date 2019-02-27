include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/sample/hello.casl
	../../../../casl2 -O $<

clean: objclean

objclean:
	$(RM) a.o
