#ifndef YACASL2_ASSEMBLE_INCLUDED
#define YACASL2_ASSEMBLE_INCLUDED

#include <stdbool.h>
#include "struct.h"
#include "word.h"

/**
 * CASL IIの仕様
 */
enum {
    LABELSIZE = 8,         /** ラベルの最大文字数 */
    OPDSIZE = 40,          /** オペラントの最大数。CASL IIシミュレータの制限 */
};

/**
 * YACASL2の制限
 */
enum {
    LINESIZE = 1024,       /* 行の最大文字数 */
    TOKENSIZE = 256,       /* トークンの最大文字数 */
};

/**
 * アセンブルモード
 */
typedef struct {
    bool src;             /* ソースを表示する場合はtrue */
    bool label;           /* ラベル表を表示する場合はtrue */
    bool onlylabel;       /* ラベル表を表示して終了する場合はtrue */
    bool asdetail;        /* アセンブラ詳細結果を表示する場合はtrue */
    bool onlyassemble;    /* アセンブルだけを行う場合はtrue */
} ASMODE;

extern ASMODE asmode;

/**
 * アセンブルのプロパティ
 */
typedef struct {
    WORD ptr;     /* 現在のポインタ */
    WORD lptr;    /* リテラル（=付きの値）を格納するポインタ */
    char *prog;   /* 他のプログラムで参照する入口名 */
} ASPROP;

extern ASPROP *asprop;

/**
  * アセンブラ命令を表す番号
  */
typedef enum {
    START = 01,
    END = 02,
    DS = 03,
    DC = 04,
} ASCMDID;

/**
  * アセンブラ命令を表す配列
  */
typedef struct {
    ASCMDID cmdid;
    int opdc_min;
    int opdc_max;
    char *cmd;
} ASCMD;

/**
 * マクロ命令を表す番号
 */
typedef enum {
    IN = 011,
    OUT = 012,
    RPUSH = 013,
    RPOP = 014,
} MACROCMDID;

/**
 * マクロ命令を表す配列
 */
typedef struct {
    MACROCMDID cmdid;
    int opdc_min;
    int opdc_max;
    char *cmd;
} MACROCMD;

/**
 * ラベル配列
 */
typedef struct {
    char *prog;
    char *label;
    WORD adr;
} LABELARRAY;

/**
 * ラベル表
 */
typedef struct _LABELTAB {
    struct _LABELTAB *next;
    char *prog;
    char *label;
    WORD adr;
} LABELTAB;

enum {
    /**
     * ラベル表のサイズ
     */
    LABELTABSIZE = 251,
};

/**
 * アセンブラが、1回目か、2回目か、を表す
 */
typedef enum {
    FIRST = 0,
    SECOND = 1,
} PASS;

/**
 * プログラム名とラベルに対応するアドレスをラベル表から検索する
 */
WORD getlabel(const char *prog, const char *label);

/**
 * ラベルを表に追加する
 */
bool addlabel(const char *prog, const char *label, WORD word);

/**
 * ラベル表を表示する
 */
void printlabel();

/**
 * ラベル表を解放する
 */
void freelabel();

/**
 * オペランド
 */
typedef struct {
    int opdc;
    char *opdv[OPDSIZE];
} OPD;

/**
 * 命令行
 */
typedef struct {
    char *label;
    char *cmd;
    OPD *opd;
} CMDLINE;

/**
 * 1行を解析する
 */
CMDLINE *linetok(const char *line);

/**
 * 指定された名前のファイルをアセンブル
 * 2回実行される
 */
bool assemble(const char *file, PASS pass);

/**
 * 指定されたファイルにアセンブル結果を書込
 */
void outassemble(const char *file);

#endif            /* YACASL2_ASSEMBLE_INCLUDEDの終端 */
