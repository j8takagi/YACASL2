include ../Define.mk
include ../Test.mk

CC := gcc
CFLAGS := -Wall -Wextra
INCLUDEDIR := ../../../../include

cmd: a.o

a.o: create_a
	./$^

create_a: create_a.c
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -o $@ $^

clean: cleanobj

cleanobj:
	rm -f create_a
	rm -f a.o
