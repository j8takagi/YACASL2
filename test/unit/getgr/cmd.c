#include <stdio.h>
#include "assemble.h"
#include "cerr.h"

WORD getgr(const char *str, bool is_x);

int main(){
    int i, j;
    char *title = malloc(64);
    WORD r;
    bool is_x[] = {
        false, true
    };
    char *str[] = {
        "", "0", "aaa", "GR", "GR8", "GR20",
        "GR0", "GR1", "GR2", "GR3", "GR4", "GR5", "GR6", "GR7"
    };
    static CERR cerr_getgr[] = {
        { 120, "GR0 in operand x" },
    };

    cerr_init();    /* エラーの初期化 */
    addcerrlist(ARRAYSIZE(cerr_getgr), cerr_getgr);
    for(i = 0; i <= 1; i++) {
        title = (is_x[i] == false) ? "Generel Register" : "Index Register";
        printf("== %s ==\n", title);
        for(j = 0; j < ARRAYSIZE(str); j++) {
            cerr->num = 0;
            r = getgr(str[j], is_x[i]);
            printf("%s\t#%04X", str[j], r);
            if(cerr->num > 0) {
                printf("\tError - %d\t%s", cerr->num, cerr->msg);
            }
            printf("\n");
        }
    }
    freecerr();
    return 0;
}
