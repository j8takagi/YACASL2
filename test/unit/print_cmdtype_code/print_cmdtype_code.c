#include "casl2.h"
#include "cerr.h"
#include <string.h>

int compare_code(const void *a, const void *b)
{
    const CMDCODEARRAY ca = **(const CMDCODEARRAY **)a;
    const CMDCODEARRAY cb = **(const CMDCODEARRAY **)b;
    int diff;
    if((diff = strcmp(ca.cmd, cb.cmd)) == 0) {
        return ca.type - cb.type;
    } else {
        return diff;
    }
}

/* 命令と命令タイプがキーのハッシュ表を表示する */
void print_cmdtype_code()
{
    int i, j = 0;
    CMDCODETAB *np;
    CMDCODEARRAY **ar;
    ar = malloc(sizeof(*ar) * cmdcodesize);
    for(i = 0; i < cmdtabsize; i++) {
        np = cmdtype_code[i];
        while(np != NULL) {
            ar[j++] = np->cca;
            np = np->next;
        }
    }
    qsort(ar, cmdcodesize, sizeof(*ar), (int (*)(const void*, const void*))compare_code);
    for(i = 0; i < cmdcodesize; i++) {
        fprintf(stdout, "%s\t0%02o\t#%04X\n", ar[i]->cmd, ar[i]->type, ar[i]->code);
    }
}

int main()
{
    create_cmdtype_code();
    print_cmdtype_code();
    free_cmdtype_code();
    if(cerrno != 0) {
        printf("\terror - %d: %s\n", cerrno, cerrmsg);
        freecerr();
        exit(-1);
    }
    return 0;
}
