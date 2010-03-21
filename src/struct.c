#include "casl2.h"

/* COMET IIのメモリ */
WORD *memory;

/* メモリサイズ */
int memsize = DEFAULT_MEMSIZE;

/* COMET IIのCPU */
CPU *cpu;

/* クロック周波数 */
int clocks = DEFAULT_CLOCKS;

/* CASL2プログラムのプロパティ */
PROGPROP *progprop;

/* COMET II仮想マシンのリセット */
void reset()
{
    int i;
    /* メモリの初期化 */
    memory = malloc_chk(memsize * sizeof(WORD), "memory");
    for(i = 0; i < memsize; i++) {
        memory[i] = 0x0;
    }
    /* CPUの初期化 */
    cpu = malloc_chk(sizeof(CPU), "cpu");
    for(i = 0; i < GRSIZE; i++) {
        cpu->gr[i] = 0x0;
    }
    cpu->sp = cpu->pr = cpu->fr = 0x0;
    /* CASL2プログラムのプロパティ */
    progprop = malloc_chk(sizeof(PROGPROP), "progprop");
}

/* COMET II仮想マシンのシャットダウン */
void shutdown()
{
    free(cpu);
    free(memory);
}
