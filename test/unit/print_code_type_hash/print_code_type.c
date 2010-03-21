#include "casl2.h"
#include "cerr.h"

/* 命令コードがキーのハッシュ表を表示する */
void print_code_type()
{
    int i;
    CMDCODETAB *np;
    for(i = 0; i < cmdtabsize; i++){
        for(np = code_type[i]; np != NULL; np = np->next) {
            fprintf(stdout, "(%2d) - #%04X\t0%02o\t%s\n",
                    i, np->cmdtypecode->code, np->cmdtypecode->type, np->cmdtypecode->cmd);
        }
    }
}

int main()
{
    /* エラーの初期化 */
    cerr = malloc_chk(sizeof(CERR), "cerr");
    /* ハッシュ表作成 */
    create_code_type();
    /* ハッシュ表表示 */
    print_code_type();
    /* ハッシュ表解放 */
    free_code_type();
    /* エラーの表示 */
    if(cerr->num != 0) {
        printf("\terror - %d: %s\n", cerr->num, cerr->msg);
        freecerr();
        exit(-1);
    }
    return 0;
}
