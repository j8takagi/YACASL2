include ../Define.mk
include ../Test.mk

CC := gcc
CFLAGS := -Wall

.INTERMEDIATE: $(CMD_FILE)

CMDSRC_FILE := cmd.c
TARGETDIR := ../../../src
INCLUDEDIR := ../../../include
TESTTARGET_FILES := $(TARGETDIR)/hash.c $(TARGETDIR)/cmd.c  $(TARGETDIR)/exec.c $(TARGETDIR)/struct.c $(TARGETDIR)/dump.c  $(TARGETDIR)/word.c $(TARGETDIR)/cmem.c  $(TARGETDIR)/cerr.c      # Set test target files

$(CMD_FILE): $(CMDSRC_FILE) $(TESTTARGET_FILES)
	$(CC) $(CFLAGS) -I $(INCLUDEDIR) -o $@ $^
