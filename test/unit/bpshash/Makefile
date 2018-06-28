include ../Define.mk
include ../Test.mk

CC := gcc
CFLAGS := -g -Wall

.INTERMEDIATE: $(CMD_FILE)

CMDSRC_FILE := cmd.c
TARGETDIR := ../../../src
INCLUDEDIR := ../../../include
TESTTARGET_FILES := $(TARGETDIR)/hash.c $(TARGETDIR)/debugger.c $(TARGETDIR)/cmem.c $(TARGETDIR)/hash.c $(TARGETDIR)/cerr.c $(TARGETDIR)/word.c $(TARGETDIR)/exec.c $(TARGETDIR)/struct.c $(TARGETDIR)/debugger.c $(TARGETDIR)/dump.c      # Set test target files

$(CMD_FILE): $(CMDSRC_FILE) $(TESTTARGET_FILES)
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -o $@ $^

clean_cmd:
	@rm -rf cmd.dSYM cmd

clean: clean_cmd
