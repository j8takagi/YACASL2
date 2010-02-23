#include "casl2.h"
#include "cerr.h"

/* 命令コードがキーのハッシュ表を表示する */
void print_code_type()
{
    int i;
    CMDCODETAB *np;
    for(i = 0; i < cmdcodesize; i++){
        for(np = code_type[i]; np != NULL; np = np->next) {
            fprintf(stdout, "(%2d) - #%04X\t0%02o\t%s\n", i, np->cca->code, np->cca->type, np->cca->cmd);
        }
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
