#include "casl2.h"

/* COMET IIのメモリ */
WORD *memory;

/* COMET IIのCPUレジスタ */
WORD GR[REGSIZE], SP, PR, FR;

/* メモリーサイズ */
int memsize = DEFAULT_MEMSIZE;

/* クロック周波数 */
int clocks = DEFAULT_CLOCKS;

/* 実行開始番地 */
WORD startptr = 0x0;

/* 実行終了番地 */
WORD endptr = 0x0;
