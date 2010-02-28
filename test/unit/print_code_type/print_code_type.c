#include "casl2.h"
#include "cerr.h"

int compare_code(const void *a, const void *b)
{
    return (**(const CMDCODEARRAY **)a).code - (**(const CMDCODEARRAY **)b).code;
}

/* 命令コードがキーのハッシュ表を表示する */
void print_code_type()
{
    int i, j = 0;
    CMDCODETAB *np;
    CMDCODEARRAY **ar;
    ar = malloc(sizeof(*ar) * cmdcodesize);
    for(i = 0; i < cmdtabsize; i++) {
        np = code_type[i];
        while(np != NULL) {
            ar[j++] = np->cca;
            np = np->next;
        }
    }
    qsort(ar, cmdcodesize, sizeof(*ar), (int (*)(const void*, const void*))compare_code);
    for(i = 0; i < cmdcodesize; i++) {
        fprintf(stdout, "#%04X\t0%02o\t%s\n", ar[i]->code, ar[i]->type, ar[i]->cmd);
    }
}

int main()
{
    create_code_type();
    print_code_type();
    free_code_type();
    if(cerrno != 0) {
        printf("\terror - %d: %s\n", cerrno, cerrmsg);
        freecerr();
        exit(-1);
    }
    return 0;
}
