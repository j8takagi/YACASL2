#include "casl2.h"

/* 命令と命令タイプがキーのハッシュ表を表示する */
void print_cmdtype_code()
{
    int i;
    CMDTAB *np;
    for(i = 0; i < comet2cmdsize; i++){
        np = cmdtype_code[i];
        while(np != NULL) {
            fprintf(stdout, "(%2d) - %s\t0%02o\t#%04X\n",
                    i, np->cmd->name, np->cmd->type, np->cmd->code);
            np = np->next;
        }
    }
}

int main(){
    /* エラーの初期化 */
    cerr = malloc_chk(sizeof(CERR), "cerr");
    /* ハッシュ表作成 */
    create_cmdtype_code();
    /* ハッシュ表表示 */
    print_cmdtype_code();
    /* ハッシュ表解放 */
    free_cmdtype_code();
    /* エラーの表示 */
    if(cerr->num != 0) {
        printf("\terror - %d: %s\n", cerr->num, cerr->msg);
        freecerr();
        exit(-1);
    }
    return 0;
}
