include ../Define.mk
include ../Test.mk

cmd: a.o

a.o: ../../../../as/cmd/XOR/xor_r1_r2__clear.casl
	../../../../casl2 -O $<

clean: cleanobj

cleanobj:
	$(RM) a.o
