#include "casl2.h"
#include "exec.h"

/* COMET IIのメモリを表示 */
void dumpmemory()
{
    const int col = 16;
    int i;
    /* Header */
    fprintf(stdout, "#%04X: adr :", cpu->pr);
    for(i = 0; i < memsize && i < col; i++) {
        fprintf(stdout, " %04X", i);
    }
    fprintf(stdout, "\n");
    /* Memory */
    for(i = 0; i < memsize; i++) {
        if(i % col == 0) {
            fprintf(stdout, "#%04X: %04X: ", cpu->pr, i);
        }
        fprintf(stdout, "%04X", memory[i]);
        if(i > 0 && (i + 1) % col == 0) {
            fprintf(stdout, "\n");
        } else {
            fprintf(stdout, " ");
        }
    }
}

/* COMET IIのレジスタを表示 */
void dspregister()
{
    int i;
    for(i = 0; i < GRSIZE; i++ ) {
        fprintf(stdout, "#%04X: GR%d: ", cpu->pr, i);
        print_dumpword(cpu->gr[i], (&execmode)->logical);
    }
    fprintf(stdout, "#%04X: SP:  %6d = #%04X = %s\n",
            cpu->pr, cpu->sp, cpu->sp, word2bit(cpu->sp));
    fprintf(stdout, "#%04X: PR:  %6d = #%04X = %s\n",
            cpu->pr, cpu->pr, cpu->pr, word2bit(cpu->pr));
    fprintf(stdout, "#%04X: FR (OF SF ZF): %s\n",
            cpu->pr, (word2bit(cpu->fr)+13));
}
