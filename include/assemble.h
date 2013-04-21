#ifndef YACASL2_ASSEMBLE_INCLUDED
#define YACASL2_ASSEMBLE_INCLUDED

#include <stdbool.h>
#include "struct.h"
#include "word.h"

/**
 * @brief CASL IIの仕様
 */
enum {
    LABELSIZE = 8,         /**<ラベルの最大文字数 */
    OPDSIZE = 40,          /**<オペラントの最大数。CASL IIシミュレータの制限 */
};

/**
 * @brief YACASL2の制限
 */
enum {
    LINESIZE = 1024,       /**<行の最大文字数 */
    TOKENSIZE = 256,       /**<トークンの最大文字数 */
};

/**
 * @brief アセンブルモードを表すデータ型
 */
typedef struct {
    bool src;             /**<ソースを表示する場合はtrue */
    bool label;           /**<ラベル表を表示する場合はtrue */
    bool onlylabel;       /**<ラベル表を表示して終了する場合はtrue */
    bool asdetail;        /**<アセンブラ詳細結果を表示する場合はtrue */
    bool onlyassemble;    /**<アセンブルだけを行う場合はtrue */
} ASMODE;

/**
 * @brief アセンブルモード: src, label, onlylabel, asdetail, onlyassemble
 */
extern ASMODE asmode;

/**
 * @brief アセンブル時の現在およびリテラルのアドレスとプログラム入口名を表すデータ型
 */
typedef struct {
    WORD ptr;     /**<現在のアドレス */
    WORD lptr;    /**<リテラル（=付きの値）のアドレス */
    char *prog;   /**<他のプログラムで参照する入口名 */
} ASPTR;

/**
 * @brief アセンブル時の、現在およびリテラルのアドレスとプログラム入口名: ptr, lptr, prog
 */
extern ASPTR *asptr;

/**
 * @brief ラベル配列を表すデータ型
 */
typedef struct {
    char *prog;                 /**<プログラム  */
    char *label;                /**<ラベル */
    WORD adr;                   /**<アドレス */
} LABELARRAY;

/**
 * @brief ラベル表を表すデータ型
 */
typedef struct _LABELTAB {
    struct _LABELTAB *next;     /**<リスト次項目へのポインタ */
    char *prog;                 /**<プログラム名  */
    char *label;                /**<ラベル名 */
    WORD adr;                   /**<アドレス */
} LABELTAB;

/**
 * ラベル表のサイズ
 */
enum {
    LABELTABSIZE = 251,         /**<ラベル表のサイズ */
};

/**
 * @brief アセンブラが、1回目か2回目かを表す数値
 */
typedef enum {
    FIRST = 0,                  /**<アセンブラ1回目 */
    SECOND = 1,                 /**<アセンブラ2回目 */
} PASS;

/**
 * @brief ラベルのエラーをエラーリストに追加する
 *
 * @return なし
 */
void addcerrlist_label();

/**
 * @brief プログラム名とラベルに対応するアドレスをラベル表から検索する
 *
 * @return プログラム名とラベルに対応するアドレス
 *
 * @param *prog プログラム名
 * @param *label ラベル
 */
WORD getlabel(const char *prog, const char *label);

/**
 * @brief プログラム名、ラベル、アドレスをラベル表に追加する
 *
 * @return 追加に成功した時はtrue、失敗した時はfalse
 *
 * @param *prog プログラム名
 * @param *label ラベル
 * @param adr アドレス
 */
bool addlabel(const char *prog, const char *label, WORD adr);

/**
 * @brief ラベル表を表示する
 *
 * @return なし
 */
void printlabel();

/**
 * @brief ラベル表を解放する
 *
 * @return なし
 */
void freelabel();

/**
 * @brief オペランドを表すデータ型
 */
typedef struct {
    int opdc;                   /**<オペランド数 */
    char *opdv[OPDSIZE];        /**<オペランド配列 */
} OPD;

/**
 * @brief 命令行を表すデータ型
 */
typedef struct {
    char *label;                /**<ラベル */
    char *cmd;                  /**<コマンド */
    OPD *opd;                   /**<オペランド */
} CMDLINE;

/**
 * @brief トークン取得のエラーを追加する
 *
 * @return なし
 */
void addcerrlist_tok();

/**
 * @brief 行から、ラベル・コマンド・オペランドを取得する
 *
 * @return ラベル・コマンド・オペランド
 *
 * @param *line 行
 */
CMDLINE *linetok(const char *line);

/**
 * @brief アセンブルエラーをエラーリストに追加する
 *
 * @return なし
 */
void addcerrlist_assemble();

/**
 * @brief 指定された名前のファイルをアセンブル\n
 *
 * 1回目ではラベルを登録し、2回目ではラベルからアドレスを読み込む
 *
 * @return アセンブル完了時はtrue、エラー発生時はfalseを返す
 *
 * @param *file ファイル名
 * @param pass アセンブラが何回目かを表す数
 */
bool assemblefile(const char *file, PASS pass);

/**
 * @brief ファイルにアセンブル結果を書き込む
 *
 * @return なし
 *
 * @param *file ファイル名
 */
void outassemble(const char *file);

#endif            /* YACASL2_ASSEMBLE_INCLUDEDの終端 */
