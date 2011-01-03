#ifndef YACASL2_CASL2_INCLUDED
#define YACASL2_CASL2_INCLUDED

#include "word.h"

/**
 * COMET IIの規格
 */
enum {
    CMDSIZE = 4,              /**<命令の最大文字数 */
    GRSIZE = 8,               /**<汎用レジスタの数。COMET II規格により0から7までの8つ */
    DEFAULT_MEMSIZE = 512,    /**<デフォルトのメモリ容量。COMET II規格では、65535語（word） */
    DEFAULT_CLOCKS = 5000000, /**<デフォルトのクロック周波数。COMET II規格では、未定義 */
};

/**
 * COMET II フラグのマスク値
 */
enum {
    OF = 0x4,    /**<Overflow Flag */
    SF = 0x2,    /**<Sign Flag */
    ZF = 0x1,    /**<Zero Flag */
};

/**
 * COMET IIのCPU
 */
typedef struct {
    WORD gr[GRSIZE]; /**<汎用レジスタ */
    WORD sp;         /**<スタックポインタ */
    WORD pr;         /**<プログラムレジスタ */
    WORD fr;         /**<フラグレジスタ */
} CPU;

/**
 * COMET IIの仮想実行マシンシステム
 */
typedef struct {
    CPU *cpu;        /**<CPU */
    WORD *memory;    /**<メモリ */
    int memsize;     /**<メモリサイズ */
    int clocks;      /**<クロック周波数 */
} SYSTEM;

extern SYSTEM *sys;

/**
 * COMET II 命令
 * 命令タイプは、オペランドにより6種類に分類
 */
typedef enum {
    /**
     *  オペランド数2または3
     *     第1オペランド: 汎用レジスタ
     *     第2オペランド: アドレス
     *     第3オペランド: 指標レジスタ
     */
    R_ADR_X = 010,
    /**
     *  オペランド数2または3
     *     第1オペランド: 汎用レジスタ、
     *     第2オペランド: アドレスに格納されている内容
     *     第3オペランド: 指標レジスタ
     */
    R_ADR_X_ = 011,
    /**
     *  オペランド数2
     *     第1オペランド: 汎用レジスタ
     *     第2オペランド: 汎用レジスタ
     */
    R1_R2 = 020,
    /**
     * オペランド数1または2
     *     第1オペランド: アドレス
     *     第2オペランド: 指標レジスタ
     */
    ADR_X = 030,
    /**
     *   オペランド数1
     *     第1オペランド: 汎用レジスタ
     */
    R_ = 040,
    /**
     *  オペランドなし
     */
    NONE = 0,
} CMDTYPE;

/**
 * 命令コード配列
 */
typedef struct {
    char *name;
    CMDTYPE type;
    WORD code;
} CMD;

/**
 * 命令コードのハッシュ表
 */
typedef struct _CMDTAB {
    struct _CMDTAB *next;
    CMD *cmd;
} CMDTAB;

/**
 * CASL2プログラムのプロパティ
 */
typedef struct {
    WORD start;   /**<プログラムの開始番地 */
    WORD end;     /**<プログラムの終了番地 */
} PROGPROP;

extern PROGPROP *prog;

/**
 * COMET II仮想マシンのリセット
 */
void reset(int memsize, int clocks);

/**
 * COMET II仮想マシンのシャットダウン
 */
void shutdown();

/**
 * 名前とタイプがキーの命令ハッシュ表を作成する
 */
bool create_cmdtype_code();

/**
 * 命令の名前とタイプから、命令コードを返す
 * 無効な場合は0xFFFFを返す
 */
WORD getcmdcode(const char *cmd, CMDTYPE type);

/**
 * 名前とタイプがキーの命令ハッシュ表を解放する
 */
void free_cmdtype_code();

/**
 * コードがキーの命令ハッシュ表を作成する
 */
bool create_code_type();

/**
 * 命令コードから命令タイプを返す
 * 無効な場合はNONEを返す
 */
CMDTYPE getcmdtype(WORD code);

/**
 * コードがキーの命令ハッシュ表を解放する
 */
void free_code_type();

#endif            /* YACASL2_CASL2_INCLUDEDの終端 */