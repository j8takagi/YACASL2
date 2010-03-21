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

    cerr = malloc_chk(sizeof(CERR), "cerr");    /* エラーの初期化 */
    addcerrlist_word();
    for(i = 0; i < sizeof(str)/sizeof(str[0]); i++) {
        cerr->num = 0;
        r = n2word(str[i]);
        printf("%s\t0x%04x", str[i], r);
        if(cerr->num > 0) {
            printf("\tError - %d\t%s", cerr->num, cerr->msg);
        }
        printf("\n");
    }
    freecerr();
    return 0;
}
