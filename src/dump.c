#include "exec.h"

/* exec.hに定義された関数群 */
void dumpmemory(WORD start, WORD end)
{
    const WORD col = 0x10;
    WORD i, j, mod = 0x0;
    /* Header */
    fprintf(stdout, "#%04X: adr :", sys->cpu->pr);
    if(end > sys->memsize) {
        end = sys->memsize;
    }
    for(i = 0; i < sys->memsize && i < col; i++) {
        fprintf(stdout, " %04X", i);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "       -------------------------------------------------------------------------------------\n");
    /* Memory */
    for(i = start; i < end; i++) {
        if((mod = i % col) == 0 || i == start) {
            fprintf(stdout, "#%04X: %04X: ", sys->cpu->pr, i);
        }
        if(i == start) {
            for(j = 0; j < mod; j++) {
                fprintf(stdout, "     ");
            }
        }
        fprintf(stdout, "%04X", (sys->memory)[i]);
        if((i > 0 && (i + 1) % col == 0) || (i + 1) == end) {
            fprintf(stdout, "\n");
        } else {
            fprintf(stdout, " ");
        }
    }
}

void dspregister()
{
    char *sp = NULL;
    char *pr = NULL;
    char *fr = NULL;
    for(int i = 0; i < GRSIZE; i++ ) {
        fprintf(stdout, "#%04X: GR%d: ", sys->cpu->pr, i);
        print_dumpword(sys->cpu->gr[i], execmode.logical);
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "#%04X: SP:  %6d = #%04X = %s\n",
            sys->cpu->pr, sys->cpu->sp, sys->cpu->sp, sp = word2bit(sys->cpu->sp));
    fprintf(stdout, "#%04X: PR:  %6d = #%04X = %s\n",
            sys->cpu->pr, sys->cpu->pr, sys->cpu->pr, pr = word2bit(sys->cpu->pr));
    fprintf(stdout, "#%04X: FR (OF SF ZF): %s\n",
            sys->cpu->pr, ((fr = word2bit(sys->cpu->fr)) + 13)); /* FRは末尾3けたを表示 */

    FREE(sp);
    FREE(pr);
    FREE(fr);
}
