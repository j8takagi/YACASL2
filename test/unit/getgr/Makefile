include ../Define.mk
include ../Test.mk

CC := gcc
CFLAGS := -Wall

.INTERMEDIATE: $(CMD_FILE)

CMDSRC_FILE := cmd.c
TARGETDIR := ../../../src
INCLUDEDIR := ../../../include
TESTTARGET_FILES := $(TARGETDIR)/hash.c $(TARGETDIR)/cmem.c $(TARGETDIR)/cerr.c $(TARGETDIR)/assemble.c $(TARGETDIR)/word.c $(TARGETDIR)/label.c $(TARGETDIR)/struct.c $(TARGETDIR)/token.c $(TARGETDIR)/exec.c $(TARGETDIR)/dump.c      # Set test target files

$(CMD_FILE): $(CMDSRC_FILE) $(TESTTARGET_FILES)
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -o $@ $^

clean_cmd:
	@rm -rf cmd.dSYM cmd

clean: clean_cmd
