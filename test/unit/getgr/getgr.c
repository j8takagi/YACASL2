#include "casl2.h"
#include "assemble.h"

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

    cerr = malloc_chk(sizeof(CERR), "cerr");    /* エラーの初期化 */
    addcerrlist_assemble();
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
