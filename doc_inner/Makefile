.PHONY: all clean doxygen-clean gnu_global-clean cmetrics-clean

CD := cd
CMETRICS := cmetrics
CP := cp
DOXYGEN := doxygen
ECHO := echo
HTAGS := htags
INSTALL := install
MV := mv
RMR := rm -rf
SED := sed
WHICH := which

DOXYGEN_DIR := doxygen
DOXYSRCDIR := ..
GNU_GLOBAL_DIR := gnu_global
GNU_GLOBAL_SRCDIR := ..
CMETRICS_DIR := cmetrics
CMETRICS_SRCDIR := ../src

# chk_file_ext: 指定されたコマンドが実在する場合にターゲットを指定。
# 用例: $(call chk_cmd_ext,cmd,target)
define chk_cmd_ext
    $(if $(strip $(shell $(WHICH) $1)),$2)
endef

all: $(call chk_cmd_ext,$(DOXYGEN),$(DOXYGEN_DIR)) $(call chk_cmd_ext,$(HTAGS),$(GNU_GLOBAL_DIR)) $(call chk_cmd_ext $(CMETRICS),$(CMETRICS_DIR))

# doxygenのHTMLドキュメント作成先は、
# ../Doxyfileで設定された以下のディレクトリーの下のhtmlディレクトリー
#   OUTPUT_DIRECTORY = doc_inner
$(DOXYGEN_DIR): $(DOXYSRCDIR)/Doxyfile ../src ../include
	$(INSTALL) -d $(DOXYGEN_DIR)
	$(CD) $(DOXYSRCDIR) && $(DOXYGEN)

$(DOXYSRCDIR)/Doxyfile: Doxyfile
	@$(CP) -vf Doxyfile $(DOXYSRCDIR)/

# htagsのHTMLドキュメント作成先は、
# 引数DIRで指定されたディレクトリーの下のHTMLディレクトリー
$(GNU_GLOBAL_DIR): ../src ../include
	$(MAKE) -C $(GNU_GLOBAL_SRCDIR) gtags
	$(CD) $(GNU_GLOBAL_SRCDIR) && $(HTAGS) -anos $(CURDIR) && $(RMR) $(CURDIR)/$@ && $(MV) $(CURDIR)/HTML $(CURDIR)/$@

$(CMETRICS_DIR):
	$(INSTALL) -d $(CMETRICS_DIR)
	$(CMETRICS) $(CMETRICS_SRCDIR) | $(SED) -e '1s/ \{2,\}/	/g' -e '2,$$s/  \{1,\}/	/g' >$(CMETRICS_DIR)/files.tsv
	$(CMETRICS) -f $(CMETRICS_SRCDIR) | $(SED) -e '1s/ \{2,\}/	/g' -e '2,$$s/  \{1,\}/	/g' >$(CMETRICS_DIR)/functions.tsv

clean: doxygen-clean gnu_global-clean cmetrics-clean

distclean: clean

doxygen-clean:
	$(RMR) $(DOXYGEN_DIR)
	$(RM) ../Doxyfile

gnu_global-clean:
	$(RMR) $(GNU_GLOBAL_DIR)

cmetrics-clean:
	$(RMR) $(CMETRICS_DIR)
