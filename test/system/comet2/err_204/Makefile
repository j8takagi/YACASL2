include ../Define.mk
include ../Test.mk

CC := gcc
CFLAGS := -Wall
INCLUDEDIR := ../../../../include

.INTERMEDIATE: create_a a.comet

a.comet: create_a
	./$^

create_a: create_a.c
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -o $@ $^

$(CMD_FILE): a.comet

clean: clean0

clean0:
	@rm -f create_a
	@rm -rf create_a.dSYM
	@rm -f a.comet
