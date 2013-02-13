.PHONY: all clean doxygen-clean gnu_global-clean cmetrics-clean

CD := cd
CMETRICS := cmetrics
CP := cp
DOXYGEN := doxygen
ECHO := echo
HTAGS := htags
INSTALL := install
MAKE := make
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

all: $(DOXYGEN_DIR) $(GNU_GLOBAL_DIR) $(CMETRICS_DIR)

# doxygenのHTMLドキュメント作成先は、
# ../Doxyfileで設定された以下のディレクトリーの下のhtmlディレクトリー
#   OUTPUT_DIRECTORY = doc_inner
$(DOXYGEN_DIR): $(DOXYSRCDIR)/Doxyfile
	$(if $(strip $(shell $(WHICH) $(DOXYGEN))),$(CD) $(DOXYSRCDIR) && $(DOXYGEN) && $(MV) $(CURDIR)/html $(CURDIR)/$@,$(ECHO) '$(DOXYGEN): not found')

$(DOXYSRCDIR)/Doxyfile:
	$(CP) -f Doxyfile $(DOXYSRCDIR)/

# htagsのHTMLドキュメント作成先は、
# 引数DIRで指定されたディレクトリーの下のHTMLディレクトリー
$(GNU_GLOBAL_DIR):
	$(MAKE) -C $(GNU_GLOBAL_SRCDIR) gtags
	$(if $(strip $(shell $(WHICH) $(HTAGS))),$(CD) $(GNU_GLOBAL_SRCDIR) && $(HTAGS) -anosx $(CURDIR) && $(MV) $(CURDIR)/HTML $(CURDIR)/$@,@$(ECHO) '$(HTAGS): not found')

$(CMETRICS_DIR):
	$(INSTALL) -d $(CMETRICS_DIR)
	$(CMETRICS) $(CMETRICS_SRCDIR) | $(SED) -e '1s/ \{2,\}/\t/g' -e '2,$$s/  \{1,\}/\t/g' >$(CMETRICS_DIR)/files.tsv
	$(CMETRICS) -f $(CMETRICS_SRCDIR) | $(SED) -e '1s/ \{2,\}/\t/g' -e '2,$$s/  \{1,\}/\t/g' >$(CMETRICS_DIR)/functions.tsv

clean: doxygen-clean gnu_global-clean cmetrics-clean

doxygen-clean:
	$(RMR) $(DOXYGEN_DIR)
	$(RM) ../Doxyfile

gnu_global-clean:
	$(RMR) $(GNU_GLOBAL_DIR)

cmetrics-clean:
	$(RMR) $(CMETRICS_DIR)
