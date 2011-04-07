#ifndef YACASL2_ASSEMBLE_INCLUDED
#define YACASL2_ASSEMBLE_INCLUDED

#include <stdbool.h>
#include "struct.h"
#include "word.h"

/**
 * CASL IIの仕様
 */
enum {
    LABELSIZE = 8,         /**<ラベルの最大文字数 */
    OPDSIZE = 40,          /**<オペラントの最大数。CASL IIシミュレータの制限 */
};

/**
 * YACASL2の制限
 */
enum {
    LINESIZE = 1024,       /**<行の最大文字数 */
    TOKENSIZE = 256,       /**<トークンの最大文字数 */
};

/**
 * アセンブルモード
 */
typedef struct {
    bool src;             /**<ソースを表示する場合はtrue */
    bool label;           /**<ラベル表を表示する場合はtrue */
    bool onlylabel;       /**<ラベル表を表示して終了する場合はtrue */
    bool asdetail;        /**<アセンブラ詳細結果を表示する場合はtrue */
    bool onlyassemble;    /**<アセンブルだけを行う場合はtrue */
} ASMODE;

extern ASMODE asmode;

/**
 * アセンブル時の、現在およびリテラルのアドレスとプログラム入口名
 */
typedef struct {
    WORD ptr;     /**<現在のアドレス */
    WORD lptr;    /**<リテラル（=付きの値）のアドレス */
    char *prog;   /**<他のプログラムで参照する入口名 */
} ASPTR;

extern ASPTR *asptr;

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
    ASCMDID cmdid;              /**<アセンブル命令のID */
    int opdc_min;               /**<最小オペランド数 */
    int opdc_max;               /**<最大オペランド数 */
    char *cmd;                  /**<コマンド名 */
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
    MACROCMDID cmdid;              /**<マクロ命令のID */
    int opdc_min;                  /**<最小オペランド数 */
    int opdc_max;                  /**<最大オペランド数 */
    char *cmd;                     /**<コマンド名 */
} MACROCMD;

/**
 * ラベル配列
 */
typedef struct {
    char *prog;                 /**<プログラム  */
    char *label;                /**<ラベル */
    WORD adr;                   /**<アドレス */
} LABELARRAY;

/**
 * ラベル表
 */
typedef struct _LABELTAB {
    struct _LABELTAB *next;     /**<リスト次項目へのポインタ */
    char *prog;                 /**<プログラム名  */
    char *label;                /**<ラベル名 */
    WORD adr;                   /**<アドレス */
} LABELTAB;

enum {
    /**
     * ラベル表のサイズ
     */
    LABELTABSIZE = 251,
};

/**
 * アセンブラが、1回目か2回目か
 */
typedef enum {
    FIRST = 0,
    SECOND = 1,
} PASS;

/**
 * ラベルのエラーをエラーリストに追加
 */
void addcerrlist_label();

/**
 * プログラム名とラベルに対応するアドレスをラベル表から検索する
 */
WORD getlabel(const char *prog, const char *label);

/**
 * プログラム名、ラベル、アドレスをラベル表に追加する
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
    int opdc;                   /**<オペランド数 */
    char *opdv[OPDSIZE];        /**<オペランド配列 */
} OPD;

/**
 * 命令行
 */
typedef struct {
    char *label;                /**<ラベル */
    char *cmd;                  /**<コマンド */
    OPD *opd;                   /**<オペランド */
} CMDLINE;

/**
 * トークン取得のエラーを追加
 */
void addcerrlist_tok();

/**
 * 空白またはタブで区切られた1行から、トークンを取得する
 */
CMDLINE *linetok(const char *line);

/**
 * アセンブルエラーをエラーリストに追加
 */
void addcerrlist_assemble();

/**
 * 指定された名前のファイルをアセンブル
 * 1回目ではラベルを登録し、2回目ではラベルからアドレスを読み込む
 * アセンブル完了時はtrue、エラー発生時はfalseを返す
 */
bool assemblefile(const char *file, PASS pass);

/**
 * 引数で指定したファイルにアセンブル結果を書込
 */
void outassemble(const char *file);

#endif            /* YACASL2_ASSEMBLE_INCLUDEDの終端 */
