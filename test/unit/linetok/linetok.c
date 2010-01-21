#include <stdio.h>
#include <malloc.h>
#include "casl2.h"
#include "assemble.h"

int main(){
    int i, j;
    char *testline[] = {
        "IOTEST START\n",
        "	OUT OBUF1,OLEN1\n",
        "	OUT OBUF2,OLEN2	;comment\n",
        "	OUT OBUF1,OLEN1 \n",
        "	OUT OBUF1,OLEN1 \n",
        "	OUT OBUF2,OLEN2	\n",
        "	OUT OBUF1, OLEN1 \n",
        "BEGIN	LD	GR1, A\n",
        "\n",
        "	;comment\n"
    };
    CMDLINE *testcl = malloc(sizeof(CMDLINE));
    
    for(i = 0; i < sizeof testline /sizeof testline[0]; i++) {
        printf("%d: %s", i, testline[i]);
        testcl = linetok(testline[i]);
        if(testcl == NULL) {
            puts("cl is NULL");
        } else {
            if(testcl->label != NULL){
                printf("cl->label: %s\n", testcl->label);
            }
            if(testcl->cmd != NULL){
                printf("cl->cmd: %s\n", testcl->cmd);
                printf("cl->opdc: %d\n", testcl->opd->opdc);
                for(j = 0; j < testcl->opd->opdc; j++) {
                    printf("cl->opdv[%d]: %s\n", j, testcl->opd->opdv[j]);
                }
            }
        }
        if(cerrno != 0) {
            printf("\terror - %d: %s\n", cerrno, cerrmsg);
            freecerr();
        }
        printf("\n");
    }
    return 0;
}
