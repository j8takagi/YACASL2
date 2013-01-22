.PHONY: all clean doxygen-clean gnu_global-clean

CD := cd
DOXYGEN := doxygen
ECHO := echo
HTAGS := htags
MAKE := make
MV := mv
RMRF := rm -rf
WHICH := which

DOXYGEN_DIR := doxygen
GNU_GLOBAL_DIR := gnu_global

all: $(DOXYGEN_DIR) $(GNU_GLOBAL_DIR)

# doxygenのHTMLドキュメント作成先は、
# ../Doxyfileで設定された以下のディレクトリーの下のhtmlディレクトリー
#   OUTPUT_DIRECTORY = doc_inner
$(DOXYGEN_DIR): ../Doxyfile
	@($(WHICH) $(DOXYGEN) && ($(CD) .. && $(DOXYGEN) && $(CD) - && $(MV) html $@) >/dev/null || $(ECHO) "$(DOXYGEN): not found")

# htagsのHTMLドキュメント作成先は、
# 引数DIRで指定されたディレクトリーの下のHTMLディレクトリー
$(GNU_GLOBAL_DIR):
	$(MAKE) -C .. gtags
	$(WHICH) $(HTAGS) && ($(CD) .. && $(HTAGS) -anosx doc_inner && $(CD) - && $(MV) HTML $@)  >/dev/null || $(ECHO) "$(HTAGS): not found"

clean: doxygen-clean gnu_global-clean

doxygen-clean:
	@$(RMRF) $(DOXYGEN_DIR)

gnu_global-clean:
	@$(RMRF) $(GNU_GLOBAL_DIR)
