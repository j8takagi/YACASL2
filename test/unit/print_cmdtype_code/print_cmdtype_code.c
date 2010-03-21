#include "casl2.h"
#include "cerr.h"
#include <string.h>

int compare_code(const void *a, const void *b)
{
    const CMD ca = **(const CMD **)a;
    const CMD cb = **(const CMD **)b;
    int diff;

    if((diff = strcmp(ca.name, cb.name)) == 0) {
        return ca.type - cb.type;
    } else {
        return diff;
    }
}

/* 命令と命令タイプがキーのハッシュ表を表示する */
void print_cmdtype_code()
{
    int i, j = 0;
    CMDTAB *np;
    CMD **ar;
    ar = malloc(sizeof(*ar) * comet2cmdsize);
    for(i = 0; i < comet2cmdsize; i++) {
        np = cmdtype_code[i];
        while(np != NULL) {
            ar[j++] = np->cmd;
            np = np->next;
        }
    }
    qsort(ar, comet2cmdsize, sizeof(*ar), (int (*)(const void*, const void*))compare_code);
    for(i = 0; i < comet2cmdsize; i++) {
        fprintf(stdout, "%s\t0%02o\t#%04X\n", ar[i]->name, ar[i]->type, ar[i]->code);
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
