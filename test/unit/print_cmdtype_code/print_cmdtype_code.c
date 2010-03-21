#include "casl2.h"
#include "cerr.h"
#include <string.h>

int compare_code(const void *a, const void *b)
{
    const CMDTYPECODE ca = **(const CMDTYPECODE **)a;
    const CMDTYPECODE cb = **(const CMDTYPECODE **)b;
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
    CMDTYPECODE **ar;
    ar = malloc(sizeof(*ar) * cmdtypecodesize);
    for(i = 0; i < cmdtabsize; i++) {
        np = cmdtype_code[i];
        while(np != NULL) {
            ar[j++] = np->cmdtypecode;
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
    /* エラーの初期化 */
    cerr = malloc_chk(sizeof(CERR), "cerr");
    /* ハッシュ表作成 */
    create_cmdtype_code();
    /* 命令表の表示 */
    print_cmdtype_code();
    /* ハッシュ表解放 */
    free_cmdtype_code();
    if(cerr->num != 0) {
        printf("\terror - %d: %s\n", cerr->num, cerr->msg);
        freecerr();
        exit(-1);
    }
    return 0;
}
