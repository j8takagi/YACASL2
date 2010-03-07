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

/* COMET II仮想マシンのリセット */
void reset()
{
    int i;
    for(i = 0; i < REGSIZE; i++) {
        GR[i] = 0x0;
    }
    SP = PR = FR = 0x0;
    memory = malloc(memsize * sizeof(WORD));
    for(i = 0; i < memsize; i++) {
        memory[i] = 0x0;
    }
}
