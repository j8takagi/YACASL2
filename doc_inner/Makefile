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
	$(if $(strip $(shell $(WHICH) $(DOXYGEN))),$(CD) .. && $(DOXYGEN) && $(MV) $(CURDIR)/html $(CURDIR)/$@,$(ECHO) '$(DOXYGEN): not found')

# htagsのHTMLドキュメント作成先は、
# 引数DIRで指定されたディレクトリーの下のHTMLディレクトリー
$(GNU_GLOBAL_DIR):
	$(MAKE) -C .. gtags
	$(if $(strip $(shell $(WHICH) $(HTAGS))),$(CD) .. && $(HTAGS) -anosx $(CURDIR) && $(MV) $(CURDIR)/HTML $(CURDIR)/$@,@$(ECHO) '$(HTAGS): not found')

clean: doxygen-clean gnu_global-clean

doxygen-clean:
	@$(RMRF) $(DOXYGEN_DIR)

gnu_global-clean:
	@$(RMRF) $(GNU_GLOBAL_DIR)
