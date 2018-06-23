#include "exec.h"

void debugger()
{
    char *buf = malloc_chk(DBINSIZE + 1, "debugger.buf");
    for( ; ;) {
        fprintf(stdout, "COMET II (Type ? for help) > ");
        fgets(buf, DBINSIZE, stdin);
        if(*buf == '\0' || *buf == 's') {
            break;
        } else if(*buf == 'c') {
            execmode.debugger = false;
            break;
        } else if(*buf == 't') {
            fprintf(stdout, "#%04X: Register::::\n", sys->cpu->pr);
            dspregister();
        } else if(*buf == 'd') {
            dumpmemory();
        } else if(*buf == '?') {
            fprintf(stdout, "s (default) -- Step by step running your program until next interaction.\n");
            fprintf(stdout, "c -- Continue running your program.\n");
            fprintf(stdout, "t -- Display CPU register.\n");
            fprintf(stdout, "d -- Display memory dump.\n");
            break;
        }
    }
}
