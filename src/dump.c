#include <stdio.h>
#include "exec.h"

/**
 * COMET IIのメモリを表示する
 */
void dumpmemory()
{
    const int col = 16;
    int i;
    /* Header */
    fprintf(stdout, "#%04X: adr :", sys->cpu->pr);
    for(i = 0; i < sys->memsize && i < col; i++) {
        fprintf(stdout, " %04X", i);
    }
    fprintf(stdout, "\n");
    /* Memory */
    for(i = 0; i < sys->memsize; i++) {
        if(i % col == 0) {
            fprintf(stdout, "#%04X: %04X: ", sys->cpu->pr, i);
        }
        fprintf(stdout, "%04X", (sys->memory)[i]);
        if(i > 0 && (i + 1) % col == 0) {
            fprintf(stdout, "\n");
        } else {
            fprintf(stdout, " ");
        }
    }
}

/**
 * COMET IIのレジスタを表示する
 */
void dspregister()
{
    int i;
    for(i = 0; i < GRSIZE; i++ ) {
        fprintf(stdout, "#%04X: GR%d: ", sys->cpu->pr, i);
        print_dumpword(sys->cpu->gr[i], (&execmode)->logical);
    }
    fprintf(stdout, "#%04X: SP:  %6d = #%04X = %s\n",
            sys->cpu->pr, sys->cpu->sp, sys->cpu->sp, word2bit(sys->cpu->sp));
    fprintf(stdout, "#%04X: PR:  %6d = #%04X = %s\n",
            sys->cpu->pr, sys->cpu->pr, sys->cpu->pr, word2bit(sys->cpu->pr));
    fprintf(stdout, "#%04X: FR (OF SF ZF): %s\n",
            sys->cpu->pr, (word2bit(sys->cpu->fr)+13));
}
