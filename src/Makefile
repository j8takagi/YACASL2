# ソースファイル。グループに分類
CMDOBJ := casl2 comet2 dumpword casl2rev comet2monitor
COMMONOBJ := word cmem cerr
STRUCTOBJ := struct hash
ASOBJ := assemble token label
EXECOBJ := exec dump monitor disassemble

# ヘッダファイル
INCLUDEDIR := ../include

# コマンド
CC := gcc
CFLAGS := -g -Wall -Wextra -I$(INCLUDEDIR)
CTAGS := ctags
ETAGS := etags
ECHO := echo
SED := sed
WHICH := which

.PHONY: all build tag clean

all: build tag

# casl2、comet2、dumpword、casl2rev、comet2monitorのビルド
build: $(CMDOBJ)

casl2: $(addsuffix .o,casl2 $(COMMONOBJ) $(STRUCTOBJ) $(ASOBJ) $(EXECOBJ))

comet2: $(addsuffix .o,comet2 $(COMMONOBJ) $(STRUCTOBJ) $(ASOBJ) $(EXECOBJ))

dumpword: $(addsuffix .o,dumpword $(COMMONOBJ))

casl2rev:$(addsuffix .o,casl2rev $(COMMONOBJ) $(STRUCTOBJ) $(ASOBJ) $(EXECOBJ))

comet2monitor:$(addsuffix .o,comet2monitor $(COMMONOBJ) $(STRUCTOBJ) $(ASOBJ) $(EXECOBJ))

# .dファイルからヘッダファイルの依存関係を取得する
# tags、check、clean、.d で終わるターゲットの場合は除く
NODEP := %tags %check %clean %.d
ifeq ($(filter $(NODEP),$(MAKECMDGOALS)),)
  -include $(addsuffix .d,$(CMDOBJ) $(COMMONOBJ) $(STRUCTOBJ) $(ASOBJ) $(EXECOBJ))
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
	$(RM) casl2 comet2 dumpword casl2rev

clean:
	$(RM) *.o *.d core tags TAGS

tag-clean:
	$(RM) TAGS tags

# ヘッダファイルの依存関係を.dファイルに出力する
%.d: %.c $(INCLUDEDIR)/package.h
	$(CC) -MM -I $(INCLUDEDIR) $< | $(SED) 's,$*\.o *:,$*.o $*.d:,g' >$@

$(INCLUDEDIR)/package.h: ../VERSION
	$(MAKE) -C .. version
