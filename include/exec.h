#ifndef YACASL2_EXEC_INCLUDED
#define YACASL2_EXEC_INCLUDED

/* コードから命令のパターンを取得 */
CMDTYPE getcmdtype(WORD code);

enum {
    INSIZE = 256    /* CASL IIの、IN命令入力領域 */
};

/* 指定されたファイルからアセンブル結果を読込 */
bool inassemble(char *file);

/* 標準入力から文字データを読込（SVC 1） */
void svcin();

/* 標準出力へ文字データを書出（SVC 2） */
void svcout();

/* ロード／論理積／論理和／排他的論理和のフラグ設定。OFは常に0 */
void setfr(WORD val);

/* 算術加算。フラグを設定して値を返す */
WORD adda(WORD val0, WORD val1);

/* 算術減算。フラグを設定して値を返す */
WORD suba(WORD val0, WORD val1);

/* 論理加算。フラグを設定して値を返す */
WORD addl(WORD val0, WORD val1);

/* 論理減算。フラグを設定して値を返す */
WORD subl(WORD val0, WORD val1);

/* 算術比較のフラグ設定。OFは常に0 */
void cpa(WORD val0, WORD val1);

/* 論理比較のフラグ設定。OFは常に0 */
void cpl(WORD val0, WORD val1);

/* 算術左シフト。フラグを設定して値を返す */
WORD sla(WORD val0, WORD val1);

/* 算術右シフト。フラグを設定して値を返す */
WORD sra(WORD val0, WORD val1);

/* 論理左シフト。フラグを設定して値を返す */
WORD sll(WORD val0, WORD val1);

/* 論理右シフト。フラグを設定して値を返す */
WORD srl(WORD val0, WORD val1);

/* 仮想マシンのリセット */
void reset();

/* コードの実行 */
void exec();

/* COMET IIのメモリを表示 */
void dumpmemory();

/* COMET IIのレジスタを表示 */
void dspregister();

#endif
