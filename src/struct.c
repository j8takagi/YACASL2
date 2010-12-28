#include "struct.h"
#include "cmem.h"

/**
 * COMET IIの仮装実行マシンシステム
 */
SYSTEM *sys;

/**
 * CASL IIプログラムのプロパティ
 */
PROGPROP *prog;

/**
 * COMET II仮想マシンのリセット
 */
void reset(int memsize, int clocks)
{
    int i;

    sys = malloc_chk(sizeof(SYSTEM), "sys");
    /* メモリサイズの設定 */
    sys->memsize = memsize;
    /* クロック周波数の設定 */
    sys->clocks = clocks;
    /* メモリの初期化 */
    sys->memory = calloc_chk(sys->memsize, sizeof(WORD), "memory");
    /* CPUの初期化 */
    sys->cpu = malloc_chk(sizeof(CPU), "cpu");
    for(i = 0; i < GRSIZE; i++) {
        sys->cpu->gr[i] = 0x0;
    }
    sys->cpu->sp = sys->cpu->pr = sys->cpu->fr = 0x0;
    /* CASL2プログラムのプロパティ */
    prog = malloc_chk(sizeof(PROGPROP), "prog");
}

/**
 * COMET II仮想マシンのシャットダウン
 */
void shutdown()
{
    free_chk(prog, "prog");
    free_chk(sys->memory, "sys.memory");
    free_chk(sys->cpu, "sys.cpu");
    free_chk(sys, "sys");
}
