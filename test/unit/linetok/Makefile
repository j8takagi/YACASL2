include ../Define.mk
include ../Test.mk

CC := gcc
CFLAGS := -g -Wall

.INTERMEDIATE: $(CMD_FILE)

CMDSRC_FILE := cmd.c
TARGETDIR := ../../../src
INCLUDEDIR := ../../../include
TESTTARGET_FILES := $(TARGETDIR)/cmem.c $(TARGETDIR)/cerr.c $(TARGETDIR)/cmd.c $(TARGETDIR)/word.c $(TARGETDIR)/struct.c $(TARGETDIR)/token.c $(TARGETDIR)/hash.c      # Set test target files

$(CMD_FILE): $(CMDSRC_FILE) $(TESTTARGET_FILES)
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -o $@ $^
	@rm -rf cmd.dSYM
