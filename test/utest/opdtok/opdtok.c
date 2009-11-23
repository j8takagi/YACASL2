#include "casl2.h"
#include "assemble.h"

int main(){
    int i, j;
    OPD *opd;
    char *str[] = {
        "", "GR0,GR1", "GR0,A", "GR1,12", "GR0,0,GR1",
        "\'aaa\',0", "\'aaa\',\'bbb\'", "\'aaa\'\',\'\'bbb\'", "\'aaa,bbb\'",
        "0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9",
        "0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0",
        "\'01234567890123456789012345678901234567\'",
        "\'012345678901234567890123456789012345678\'"
    };
    for(i = 0; i < sizeof str / sizeof str[0]; i++) {
        printf("%s\n", str[i]);
        opd = opdtok(str[i]);
        printf("OPDC:%d\n", opd->opdc);
        for(j = 0; j < opd->opdc; j++) {
            printf("OPDC[%d]:%s\n", j, opd->opdv[j]);
        }
        if(cerrno > 0){
            printf("Error - %d: %s\n", cerrno, cerrmsg);
            freecerr();
        }
        printf("\n");        
    }
    return 0;
}
