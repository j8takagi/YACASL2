include ../Define.mk
include ../Test.mk

INCLUDEDIR := ../../../../include
CC := gcc
CFLAGS := -Wall -Wextra -I$(INCLUDEDIR)

.INTERMEDIATE: create_a

cmd: a.o

a.o: create_a
	./$<

clean: cleanobj

cleanobj:
	$(RM) a.o

