#ifndef YACASL2_CASL2_INCLUDED
#define YACASL2_CASL2_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "word.h"
#include "hash.h"
#include "cerr.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE(array) (sizeof(array)/sizeof(array[0]))
#endif

/* COMET IIの規格 */
enum {
    CMDSIZE = 4,              /* 命令の最大文字数 */
    GRSIZE = 8,               /* 汎用レジスタの数。COMET II規格で、7 */
    DEFAULT_MEMSIZE = 512,    /* デフォルトのメモリ容量。COMET II規格では、65536語 */
    DEFAULT_CLOCKS = 5000000, /* デフォルトのクロック周波数。COMET II規格では、未定義 */
};

/* COMET IIのメモリ */
extern WORD *memory;

/* メモリサイズ */
extern int memsize;

/* COMET II フラグのマスク値 */
enum {
    OF = 0x4,    /* Overflow Flag */
    SF = 0x2,    /* Sign Flag */
    ZF = 0x1,    /* Zero Flag */
};

/* COMET IIのCPU */
typedef struct {
    WORD gr[GRSIZE]; /* 汎用レジスタ */
    WORD sp;         /* スタックポインタ */
    WORD pr;         /* プログラムレジスタ */
    WORD fr;         /* フラグレジスタ */
} CPU;

/* COMET IIのCPU */
extern CPU *cpu;

/* クロック周波数 */
extern int clocks;

/* COMET II 命令 */
/* 命令タイプは、オペランドにより6種類に分類 */
typedef enum {
    /* オペランド数2または3 */
    /*     第1オペランド: 汎用レジスタ */
    /*     第2オペランド: アドレス */
    /*     第3オペランド: 指標レジスタ */
    R_ADR_X = 010,
    /* オペランド数2または3 */
    /*     第1オペランド: 汎用レジスタ、*/
    /*     第2オペランド: アドレスに格納されている内容 */
    /*     第3オペランド: 指標レジスタ */
    R_ADR_X_ = 011,
    /* オペランド数2 */
    /*     第1オペランド: 汎用レジスタ */
    /*     第2オペランド: 汎用レジスタ */
    R1_R2 = 020,
    /* オペランド数1または2 */
    /*     第1オペランド: アドレス */
    /*     第2オペランド: 指標レジスタ */
    ADR_X = 030,
    /* オペランド数1 */
    /*     第1オペランド: 汎用レジスタ */
    R_ = 040,
    /* オペランドなし */
    NONE = 0,
} CMDTYPE;

/* 命令コード配列 */
typedef struct {
    char *name;
    CMDTYPE type;
    WORD code;
} CMD;

/* 命令コード配列のサイズ */
extern int comet2cmdsize;

/* 命令コードのハッシュ表 */
typedef struct _CMDTAB {
    struct _CMDTAB *next;
    CMD *cmd;
} CMDTAB;

extern CMDTAB **cmdtype_code;
extern CMDTAB **code_type;
extern int cmdtabsize;

/* CASL2プログラムのプロパティ */
typedef struct {
    WORD start;   /* プログラムの開始番地 */
    WORD end;     /* プログラムの終了番地 */
} PROGPROP;

extern PROGPROP *progprop;

/* COMET II仮想マシンのリセット */
void reset();

/* COMET II仮想マシンのシャットダウン */
void shutdown();

/* 命令と命令タイプがキーのハッシュ表を作成する */
bool create_cmdtype_code();

/* 命令と命令タイプから、命令コードを取得する */
/* 無効な場合は0xFFFFを返す */
WORD getcmdcode(const char *cmd, CMDTYPE type);

/* 命令と命令タイプがキーのハッシュ表を表示する */
void print_cmdtype_code();

/* 命令と命令タイプがキーのハッシュ表を解放する */
void free_cmdtype_code();

/* 命令コードがキーのハッシュ表を作成する */
bool create_code_type();

/* 命令コードから命令タイプを取得する */
/* 無効な場合はNONEを返す */
CMDTYPE getcmdtype(WORD code);

/* 命令コードがキーのハッシュ表を表示する */
void print_code_type();

/* 命令コードがキーのハッシュ表を解放する */
void free_code_type();

/* 指定されたファイルにアセンブル結果を書込 */
void outassemble(const char *file);

#endif            /* YACASL2_CASL2_INCLUDEDの終端 */
