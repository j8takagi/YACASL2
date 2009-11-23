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
    for(i = 0; i <= 1; i++) {
        title = (is_x[i] == false) ? "Generel Register" : "Index Register";
        printf("== %s ==\n", title);
        for(j = 0; j < sizeof(str)/sizeof(str[0]); j++) {
            r = getgr(str[j], is_x[i]);
            printf("%s\t#%04X", str[j], r);
            if(cerrno > 0) {
                printf("\tError - %d\t%s", cerrno, cerrmsg);
                freecerr();
            }
            printf("\n");
        }
    }
    return 0;
}
