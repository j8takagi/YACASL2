#include "struct.h"
#include "cmem.h"

/**
 * COMET IIの仮想実行マシンシステム
 */
SYSTEM *sys;

/**
 * プログラム実行時の開始と終了のアドレス
 */
EXECPTR *execptr;

/**
 * COMET II仮想マシンのリセット
 */
void reset(int memsize, int clocks)
{
    int i;

    sys = malloc_chk(sizeof(SYSTEM), "sys");
    /* メモリサイズを設定 */
    sys->memsize = memsize;
    /* クロック周波数を設定 */
    sys->clocks = clocks;
    /* メモリを初期化 */
    sys->memory = calloc_chk(sys->memsize, sizeof(WORD), "memory");
    /* CPUを初期化 */
    sys->cpu = malloc_chk(sizeof(CPU), "cpu");
    for(i = 0; i < GRSIZE; i++) {
        sys->cpu->gr[i] = 0x0;
    }
    sys->cpu->sp = sys->cpu->pr = sys->cpu->fr = 0x0;
    /* CASL2プログラムの開始と終了のアドレスを初期化 */
    execptr = malloc_chk(sizeof(EXECPTR), "prog");
}

/**
 * COMET II仮想マシンのシャットダウン
 */
void shutdown()
{
    FREE(execptr);
    FREE(sys->memory);
    FREE(sys->cpu);
    FREE(sys);
}
