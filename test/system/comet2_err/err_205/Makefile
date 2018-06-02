include ../Define.mk
include ../Test.mk

CC := gcc
CFLAGS := -Wall
INCLUDEDIR := ../../../../include

.INTERMEDIATE: create_a a.o

a.o: create_a
	./$^

create_a: create_a.c
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -o $@ $^

$(CMD_FILE): a.o

clean: clean0

clean0:
	@rm -f create_a
	@rm -f a.o
