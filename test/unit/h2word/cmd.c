#include <stdio.h>
#include "casl2.h"
#include "assemble.h"
#include "exec.h"

int main(){
    int i;
    WORD r;
    char *str[] = {
        "#32768", "#-1", "#G", "#FFFF", "#0", "#1", "#ab", "#AB", "#20"
    };

    cerr = malloc_chk(sizeof(CERR), "cerr");    /* エラーの初期化 */
    addcerrlist_word();
    for(i = 0; i < sizeof(str)/sizeof(str[0]); i++) {
        cerr->num = 0;
        r = h2word(str[i]);
        printf("%s\t#%04X", str[i], r);
        if(cerr->num > 0) {
            printf("\tError - %d\t%s", cerr->num, cerr->msg);
        }
        printf("\n");
    }
    freecerr();
    return 0;
}
