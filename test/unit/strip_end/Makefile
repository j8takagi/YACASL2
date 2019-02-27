include ../Define.mk
include ../Test.mk

CC := gcc
CFLAGS := -Wall -Wextra

.INTERMEDIATE: $(CMD_FILE)

CMDSRC_FILE := cmd.c
TARGETDIR := ../../../src
INCLUDEDIR := ../../../include
TESTTARGET_FILES := $(TARGETDIR)/cmem.c         # Set test target files

$(CMD_FILE): $(CMDSRC_FILE) $(TESTTARGET_FILES)
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -o $@ $^

clean_cmd:
	@rm -rf cmd.dSYM cmd

clean: clean_cmd
