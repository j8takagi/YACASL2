#include <stdio.h>
#include "token.h"

int main(){
    char s[][50] = {
        "        LD      GR1,GR2",
        "        LD      GR1,GR2 ; CASL II comment",
        "        LD      GR1,='#' ; CASL II comment",
        "        LD      GR1,=';'",
        "        LD      GR1,=''';'",
        "        LD      GR1,=''';' ; CASL II comment",
    };

    for(size_t i = 0; i < sizeof(s) / sizeof(s[0]); i++) {
        printf("%s::\n", strip_casl2_comment(s[i]));
    }
    return 0;
}
