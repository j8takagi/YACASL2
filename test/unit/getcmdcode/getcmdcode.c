#include "casl2.h"

int main(){
    int i;
    WORD code;
    struct CMDCODELIST {
        char *cmd;
        CMDTYPE type;
    } cmdcodelist[] = {
        { "LD", NONE }, { "LD", 066 }, { "NOEX", R1_R2 },
        { "NOP", NONE }, { "LD", R_ADR_X_ }, { "ST", R_ADR_X },
        { "LAD", R_ADR_X }, { "LD", R1_R2 }, { "ADDA", R_ADR_X_ },
        { "SUBA", R_ADR_X_ }, { "ADDL", R_ADR_X_ }, { "SUBL", R_ADR_X_ },
        { "ADDA", R1_R2 }, { "SUBA", R1_R2 }, { "ADDL", R1_R2 },
        { "SUBL", R1_R2 }, { "AND", R_ADR_X_ }, { "OR", R_ADR_X_ },
        { "XOR", R_ADR_X_ }, { "AND", R1_R2 }, { "OR", R1_R2 },
        { "XOR", R1_R2 }, { "CPA", R_ADR_X_ }, { "CPL", R_ADR_X_ },
        { "CPA", R1_R2 }, { "CPL", R1_R2 }, { "SLA", R_ADR_X },
        { "SRA", R_ADR_X }, { "SLL", R_ADR_X }, { "SRL", R_ADR_X },
        { "JMI", ADR_X }, { "JNZ", ADR_X }, { "JZE", ADR_X },
        { "JUMP", ADR_X }, { "JPL", ADR_X }, { "JOV", ADR_X },
        { "PUSH", ADR_X }, { "POP", R_ }, { "CALL", ADR_X },
        { "SVC", ADR_X }, { "RET", NONE }
    };
    create_cmdtype_code();
    cerr = malloc_chk(sizeof(CERR), "cerr");    /* エラーの初期化 */
    for(i = 0; i < sizeof(cmdcodelist)/sizeof(cmdcodelist[0]); i++) {
        code = getcmdcode(cmdcodelist[i].cmd, cmdcodelist[i].type);
        printf("%s:0%02o ---> #%04X\n", cmdcodelist[i].cmd, cmdcodelist[i].type, code);
        if(cerr->num != 0) {
            printf("\terror - %d: %s", cerr->num, cerr->msg);
        }
    }
    freecerr();
    free_cmdtype_code();
    return 0;
}
