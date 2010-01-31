#include "casl2.h"

/* COMET IIのメモリを表示 */
void dumpmemory()
{
    const int col = 16;
    int i;
    /* Header */
    fprintf(stdout, "#%04X: adr :", PR);
    for(i = 0; i < col; i++) {
        fprintf(stdout, " %04X", i);
    }
    fprintf(stdout, "\n");
    /* Memory */
    for(i = 0; i < memsize; i++) {
        if(i % col == 0) {
            fprintf(stdout, "#%04X: %04X: ", PR, i);
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
    for(i = 0; i < REGSIZE; i++ ) {
        if(logicalmode == true) {
            fprintf(stdout, "#%04X: GR%d: %6d = #%04X = %s\n",
                    PR, i, GR[i], GR[i], word2bit(GR[i]));
        } else {
            fprintf(stdout, "#%04X: GR%d: %6d = #%04X = %s\n",
                    PR, i, (short)GR[i], GR[i], word2bit(GR[i]));
        }
    }
    fprintf(stdout, "#%04X: SP:  %6d = #%04X = %s\n", PR, SP, SP, word2bit(SP));
    fprintf(stdout, "#%04X: PR:  %6d = #%04X = %s\n", PR, PR, PR, word2bit(PR));
    fprintf(stdout, "#%04X: FR (OF SF ZF): %s\n", PR, (word2bit(FR)+13));
}
