#include "casl2.h"

/* 命令と命令タイプがキーのハッシュ表を表示する */
void print_cmdtype_code()
{
    int i;
    CMDCODETAB *np;
    for(i = 0; i < cmdcodesize; i++){
        np = cmdtype_code[i];
        while(np != NULL) {
            fprintf(stdout, "(%2d) - %s\t0%02o\t#%04X\n",
                    i, np->cca->cmd, np->cca->type, np->cca->code);
            np = np->next;
        }
    }
}

int main(){
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
