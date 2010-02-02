#include <stdio.h>
#include "casl2.h"
#include "assemble.h"
#include "exec.h"

int main(){
    int i;
    WORD r;
    char *str[] = {
        "str0", "str1", "str2", "str3", "str4", "str5", "str6", ...
    };
    for(i = 0; i < sizeof(str)/sizeof(str[0]); i++) {
        r = ##testfunc##(str[i]);
        printf("%s\t0x%04x", str[i], r);
        if(cerrno > 0) {
            printf("\tError - %d\t%s", cerrno, cerrmsg);
            freecerr();
        }
        printf("\n");
    }
    return 0;
}
