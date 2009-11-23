#include <stdio.h>
#include "casl2.h"

int main(){
    int i;
    CMDTYPE type;
    WORD codelist[] = {
        0xFFFF, 0x0001,
        0x0, 0x1000, 0x1100, 0x1200, 0x1400,
        0x2000, 0x2100, 0x2200, 0x2300, 0x2400,
        0x2500, 0x2600, 0x2700, 0x3000, 0x3100,
        0x3200, 0x3400, 0x3500, 0x3600, 0x4000,
        0x4100, 0x4400, 0x4500, 0x5000, 0x5100,
        0x5200, 0x5300, 0x6100, 0x6200, 0x6300,
        0x6400, 0x6500, 0x6600, 0x7000, 0x7100,
        0x8000, 0xF000, 0x8100
    };
    create_code_type();
    for(i = 0; i < sizeof(codelist)/sizeof(codelist[0]); i++) {
        type = getcmdtype(codelist[i]);
        printf("0x%04x ---> 0%02o\n", codelist[i], type);
        if(cerrno != 0) {
            printf("\t%s", cerrmsg);
            freecerr();
        }
    }
    free_code_type();
    return 0;
}
