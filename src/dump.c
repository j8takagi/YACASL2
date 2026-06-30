#include "exec.h"
#include "struct.h"
#include "disassemble.h"

/* exec.hに定義された関数群 */
void dumpmemory(WORD start, WORD end)
{
    const WORD col = 0x10;
    WORD mod = 0;
    /* Header */
    fprintf(stdout, "#%04X: adr :", sys->cpu->pr);
    if(end > sys->memsize) {
        end = sys->memsize;
    }
    for(int i = 0; i < sys->memsize && i < col; i++) {
        fprintf(stdout, " %04X", i);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "       -------------------------------------------------------------------------------------\n");
    /* Memory */
    for(int i = start; i < end; i++) {
        mod = i % col;
        if(mod == 0 || i == start) {
            fprintf(stdout, "#%04X: %04X: ", sys->cpu->pr, i);
        }
        if(i == start) {
            for(int j = 0; j < mod; j++) {
                fprintf(stdout, "     ");
            }
        }
        fprintf(stdout, "%04X", sys->memory[i]);
        if((i > 0 && (i + 1) % col == 0) || (i + 1) == end) {
            fprintf(stdout, "\n");
        } else {
            fprintf(stdout, " ");
        }
    }
}

void dspregister()
{
    char *sp = word2bit(sys->cpu->sp);
    char *pr = word2bit(sys->cpu->pr);
    char *fr = word2bit(sys->cpu->fr);
    enum {
        L3BIT = 16 - 3,     /* WORD値16ビットのうち、下位3ビットの開始位置 */
    };
    for(int i = 0; i < GRSIZE; i++ ) {
        fprintf(stdout, "#%04X: GR%d: ", sys->cpu->pr, i);
        print_dumpword(sys->cpu->gr[i], execmode.logical);
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "#%04X: SP:  %6d = #%04X = %s\n", sys->cpu->pr, sys->cpu->sp, sys->cpu->sp, sp);
    fprintf(stdout, "#%04X: PR:  %6d = #%04X = %s\n", sys->cpu->pr, sys->cpu->pr, sys->cpu->pr, pr);
    fprintf(stdout, "#%04X: FR (OF SF ZF): %s\n", sys->cpu->pr, fr + L3BIT); /* FRは下位3けたを表示 */
    FREE(sp);
    FREE(pr);
    FREE(fr);
}

void dsp_trace_dump()
{
    if(execptr->stop == false && (execmode.dump == true || execmode.trace == true || execmode.reverse == true)) {      /* traceまたはdumpオプション指定時、改行を出力 */
        if(sys->cpu->pr == execptr->start && execmode.reverse == true) { /* reverseオプション指定時、スタート時にプログラム全体を逆アセンブル */
            disassemble_memory(sys->memory, execptr->start, (execptr->end)-1);
        }
        if(execmode.trace == true) { /* traceオプション指定時、レジスタを出力 */
            fprintf(stdout, "#%04X: Register::::\n", sys->cpu->pr);
            dspregister();
        }
        if(execmode.dump == true) { /* dumpオプション指定時、メモリを出力 */
            fprintf(stdout, "#%04X: Memory::::\n", sys->cpu->pr);
            dumpmemory(execmode.dump_start, execmode.dump_end);
        }
        if(execmode.reverse == true) { /* reverseオプション指定時、ステップごとに語長分を逆アセンブル */
            fprintf(stdout, "#%04X: Disassemble::::", sys->cpu->pr);
            disassemble_memory(sys->memory, sys->cpu->pr, sys->cpu->pr + code2cmdwordlen(sys->memory[sys->cpu->pr] & 0xFF00) - 1);
        }
        fprintf(stdout, "\n");
    }
}
