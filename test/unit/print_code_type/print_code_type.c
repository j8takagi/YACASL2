#include "casl2.h"
#include "cerr.h"

int compare_code(const void *a, const void *b)
{
    return (**(const CMDTYPECODE **)a).code - (**(const CMDTYPECODE **)b).code;
}

/* 命令コードがキーのハッシュ表を表示する */
void print_code_type()
{
    int i, j = 0;
    CMDCODETAB *np;
    CMDTYPECODE **ar;
    ar = malloc(sizeof(*ar) * cmdtypecodesize);
    for(i = 0; i < cmdtabsize; i++) {
        np = code_type[i];
        while(np != NULL) {
            ar[j++] = np->cmdtypecode;
            np = np->next;
        }
    }
    qsort(ar, cmdtypecodesize, sizeof(*ar), (int (*)(const void*, const void*))compare_code);
    for(i = 0; i < cmdtypecodesize; i++) {
        fprintf(stdout, "#%04X\t0%02o\t%s\n", ar[i]->code, ar[i]->type, ar[i]->cmd);
    }
}

int main()
{
    /* エラーの初期化 */
    cerr = malloc_chk(sizeof(CERR), "cerr");
    /* ハッシュ表作成 */
    create_code_type();
    /* 命令表表示 */
    print_code_type();
    /* ハッシュ表削除 */
    free_code_type();
    /* エラーの表示 */
    if(cerr->num != 0) {
        printf("\terror - %d: %s\n", cerr->num, cerr->msg);
        freecerr();
        exit(-1);
    }
    return 0;
}
