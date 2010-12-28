#include <stdio.h>
#include "assemble.h"
#include "test_h2word.h"
#include "cerr.h"
#include "word.h"

int main(){
    int i;
    WORD r;
    char *str[] = {
        "#32768", "#-1", "#G", "#FFFF", "#0", "#1", "#ab", "#AB", "#20"
        "0", "01", "1a", "-5G", "123", "32767", "32768", "32769",
        "-1", "-2345", "-32768", "-32769", "-32770"
    };

    cerr_init();    /* エラーの初期化 */
    for(i = 0; i < sizeof(str)/sizeof(str[0]); i++) {
        cerr->num = 0;
        r = nh2word(str[i]);
        printf("%s\t#%04X", str[i], r);
        if(cerr->num > 0) {
            printf("\tError - %d\t%s", cerr->num, cerr->msg);
        }
        printf("\n");
    }
    freecerr();
    return 0;
}
