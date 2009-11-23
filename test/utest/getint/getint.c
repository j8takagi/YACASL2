#include <stdio.h>
#include "casl2.h"
#include "assemble.h"

int main(){
    int i;
    WORD r;
    char *str[] = {
        "0", "01", "1a", "-5G", "123", "32767", "32768", "32769",
        "-1", "-2345", "-32768", "-32769", "-32770"
    };
    for(i = 0; i < sizeof(str)/sizeof(str[0]); i++) {
        r = getint(str[i]);
        printf("%s\t0x%04x", str[i], r);
        if(cerrno > 0) {
            printf("\tError - %d\t%s", cerrno, cerrmsg);
            freecerr();
        }
        printf("\n");
    }
    return 0;
}
