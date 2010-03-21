#include <stdio.h>
#include "casl2.h"
#include "assemble.h"
#include "exec.h"

int main(){
    CERRLIST *p;

    addcerrlist_word();
    addcerrlist_assemble();
    addcerrlist_exec();
    cerr = malloc_chk(sizeof(CERR), "cerr");    /* エラーの初期化 */
    for(p = cerrlist; p != NULL; p = p->next) {
        setcerr(p->cerr->num, NULL);
        printf("%d: %s\n", cerr->num, cerr->msg);
    }
    freecerr();
    return 0;
}
