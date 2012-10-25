.PHONY: all clean clean_doxygen clean_gnu_global
CD ?= cd
MV ?= mv
MAKE ?= make
DOXYGEN ?= doxygen
HTAGS ?= htags
RMRF ?= rm -rf
PWD ?= pwd
WHICH ?= which
ECHO ?= echo

DOXYGEN_DIR := doxygen
GNU_GLOBAL_DIR := gnu_global

all: $(DOXYGEN_DIR) $(GNU_GLOBAL_DIR)

# doxygenのHTMLドキュメント作成先は、
# ../Doxyfileで設定された以下のディレクトリーの下のhtmlディレクトリー
#   OUTPUT_DIRECTORY = doc_inner
$(DOXYGEN_DIR): ../Doxyfile
	$(WHICH) $(DOXYGEN) && ($(CD) .. && $(DOXYGEN) && $(CD) - && $(MV) html $@) >/dev/null || $(ECHO) "$(DOXYGEN): not found"

# htagsのHTMLドキュメント作成先は、
# 引数DIRで指定されたディレクトリーの下のHTMLディレクトリー
$(GNU_GLOBAL_DIR):
	$(MAKE) -C .. gtags
	$(WHICH) $(HTAGS) && ($(CD) .. && $(HTAGS) -anosx doc_inner && $(CD) - && $(MV) HTML $@)  >/dev/null || $(ECHO) "$(HTAGS): not found"

clean: clean_doxygen clean_gnu_global

clean_doxygen:
	@$(RMRF) $(DOXYGEN_DIR)

clean_gnu_global:
	@$(RMRF) $(GNU_GLOBAL_DIR)
