#ifndef YACASL2_EXEC_INCLUDED
#define YACASL2_EXEC_INCLUDED

enum {
    INSIZE = 256    /* CASL IIの、IN命令入力領域 */
};

/* 実行モード */
typedef struct {
    bool trace;           /* レジストリの内容を表示する場合はtrue */
    bool logical;         /* レジストリの内容を論理値（0〜65535）で表示する場合はtrue */
    bool dump;            /* メモリの内容を表示する場合はtrue */
} EXECMODE;

extern EXECMODE execmode;

/* コードから命令のパターンを取得 */
CMDTYPE getcmdtype(WORD code);

/* 実行のエラー定義 */
bool addcerrlist_exec();

/* COMET II仮想マシンのリセット */
void reset();

/* コードの実行 */
bool exec();

/* COMET IIのメモリを表示 */
void dumpmemory();

/* COMET IIのレジスタを表示 */
void dspregister();

#endif            /* YACASL2_EXEC_INCLUDEDの終端 */
