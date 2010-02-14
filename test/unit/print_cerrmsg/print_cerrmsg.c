#include <stdio.h>
#include "casl2.h"

int main(){
    int i, num;

    for(i = 0; (num = cerr[i].num) > 0; i++) {
        setcerr(num, NULL);
        printf("%d: %s\n", cerrno, cerrmsg);
        freecerr();
    }
    return 0;
}
