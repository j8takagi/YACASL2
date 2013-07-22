INCLUDEDIR := ../include
INCLUDE = -I $(INCLUDEDIR)

# コマンド
CC := gcc
CFLAGS := -g -Wall -Wextra $(INCLUDE)
CTAGS := ctags
ETAGS := etags
ECHO := echo
SED := sed
WHICH := which

# ソースの分類
CMDOBJ := casl2 comet2 dumpword
COMMONOBJ := word cmem cerr
CASL2OBJ := struct hash
ASOBJ := assemble token label
EXECOBJ := exec dump

.PHONY: all build tag clean

all: build tag

# casl2、comet2、dumpwordのビルド
build: $(CMDOBJ)

casl2: casl2.o $(addsuffix .o,$(COMMONOBJ) $(CASL2OBJ) $(ASOBJ) $(EXECOBJ))

comet2: comet2.o $(addsuffix .o,$(COMMONOBJ) $(CASL2OBJ) $(EXECOBJ))

dumpword: dumpword.o $(addsuffix .o,$(COMMONOBJ))

# .dファイルからヘッダファイルの依存関係を取得する
# tags、check、cleanで終わるターゲットの場合は除く
ifeq (,$(strip $(filter %tags,$(MAKECMDGOALS)) $(filter %check,$(MAKECMDGOALS))  $(filter %clean,$(MAKECMDGOALS))))
  -include $(addsuffix .d,$(CMDOBJ) $(COMMONOBJ) $(CASL2OBJ) $(ASOBJ) $(EXECOBJ))
endif

# tagファイル作成 - ctags & etags
# コマンドがない場合はなにもしない
tag: tags TAGS

tags: $(INCLUDEDIR)/*.h *.c
	$(if $(strip $(shell $(WHICH) $(CTAGS))),$(CTAGS) $^,@$(ECHO) '$(CTAGS): not found')

TAGS: $(INCLUDEDIR)/*.h *.c
	$(if $(strip $(shell $(WHICH) $(ETAGS))),$(ETAGS) $^,@$(ECHO) '$(ETAGS): not found')

# makeで生成したファイルの削除
distclean: clean
	@$(RM) casl2 comet2 dumpword

clean: tag-clean
	@$(RM) *.o *.d

tag-clean:
	@$(RM) TAGS tags

# ヘッダファイルの依存関係を.dファイルに出力する
%.d: %.c
	$(CC) -MM -I $(INCLUDEDIR) $< | $(SED) 's,$*\.o *:,$*.o $*.d:,g' >$@
