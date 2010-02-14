#include <stdio.h>
#include "casl2.h"

int main(){
    int i, j;
    int code[] = {
        101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
        111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
        121, 122, 123, 124, 201, 202, 203, 204, 205, 206, 207, 999
    };
    const char *str[] = {NULL, "foobar"};

    for(i = 0; i < sizeof(str)/sizeof(str[0]); i++) {
        for(j = 0; j < sizeof(code)/sizeof(code[0]); j++) {
            setcerr(code[j], str[i]);
            printf("%d: %s - %d\t%s\n", code[j], str[i], cerrno, cerrmsg);
            if(cerrno != 0) {
                freecerr();
            }
        }
    }
    return 0;
}
