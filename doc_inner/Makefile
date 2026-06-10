.PHONY: all clean distclean doxygen-clean gnu_global-clean

CD := cd
CP := cp
DOXYGEN := doxygen
ECHO := echo
HTAGS := htags
LS := ls
MKDIR := mkdir
MV := mv
SED := sed
WHICH := which

SRCDIR := ..
DOXYGENSTAMP := doxygen___stamp
GNUGLOBALSTAMP := gnu_global___stamp

# chk_file_ext: 指定されたコマンドが実在する場合にターゲットを指定。
# 用例: $(call chk_cmd_ext,cmd,target)
define chk_cmd_ext
    $(if $(strip $(shell $(WHICH) $1)),$2)
endef

all: $(call chk_cmd_ext,$(DOXYGEN),doxygen___stamp) $(call chk_cmd_ext,$(HTAGS),gnu_global___stamp)

# doxygenのHTMLドキュメント作成先は、
# ../Doxyfileで設定された以下のディレクトリーの下のhtmlディレクトリー
#   OUTPUT_DIRECTORY = doc_inner
doxygen___stamp: $(SRCDIR)/src $(SRCDIR)/include $(SRCDIR)/Doxyfile | doxygen
	$(CD) $(SRCDIR) && $(DOXYGEN)
	$(LS) -laR doxygen >$@

doxygen:
	$(MKDIR) $@

Doxyfile:
	$(DOXYGEN) -u $@

$(SRCDIR)/Doxyfile: | Doxyfile
	@$(CP) -vf Doxyfile $(SRCDIR)/

# htagsのHTMLドキュメント作成先は、
# 引数DIRで指定されたディレクトリーの下のHTMLディレクトリー
gnu_global___stamp: $(SRCDIR)/src $(SRCDIR)/include | gnu_global
	$(MAKE) -C $(SRCDIR) gtags
	$(CD) $(SRCDIR) && $(HTAGS) -anos $(CURDIR)
	$(RM) -r gnu_global
	$(MV) HTML gnu_global
	$(LS) -laR gnu_global >$@

gnu_global:
	$(MKDIR) $@

clean:
	$(RM) *___stamp

distclean: clean doxygen-clean gnu_global-clean

doxygen-clean:
	$(RM) -r doxygen
	$(RM) ../Doxyfile

gnu_global-clean:
	$(RM) -r gnu_global
