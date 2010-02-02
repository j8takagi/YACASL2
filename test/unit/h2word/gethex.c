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
    for(i = 0; i < sizeof(str)/sizeof(str[0]); i++) {
        r = h2word(str[i]);
        printf("%s\t#%04X", str[i], r);
        if(cerrno > 0) {
            printf("\tError - %d\t%s", cerrno, cerrmsg);
            freecerr();
        }
        printf("\n");
    }
    return 0;
}
