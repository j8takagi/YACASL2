include ../Define.mk
include ../Test.mk

cmd: io.o

io.o: ../../../../as/misc/io.casl
	../../../../casl2 -O$@ $<
