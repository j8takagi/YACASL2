#include "casl2.h"
#include "exec.h"
#define _GNU_SOURCE
#include <getopt.h>

/* 指定されたファイルからCOMET II仮想メモリ（アセンブル結果）を読込 */
bool inassemble(char *file) {
    FILE *fp;
    if((fp = fopen(file, "r")) == NULL) {
        perror(file);
        return false;
    }
    fread(memory, sizeof(WORD), MEMSIZE, fp);
    fclose(fp);
    return true;
}

static struct option longopts[] = {
    {"trace", no_argument, NULL, 't'},
    {"tracearithmetic", no_argument, NULL, 't'},
    {"tracelogical", no_argument, NULL, 'T'},
    {"dump", no_argument, NULL, 'd'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0}
};

int main(int argc, char *argv[])
{
    int opt;
    const char *usage = "Usage: %s [-tTdh] FILE\n";

    while((opt = getopt_long(argc, argv, "tTdh", longopts, NULL)) != -1) {
        switch(opt) {
        case 't':
            tracemode = true;
            break;
        case 'T':
            tracemode = true;
            logicalmode = true;
            break;
        case 'd':
            dumpmode = true;
            break;
        case 'h':
            fprintf(stdout, usage, argv[0]);
            exit(-1);
        case '?':
            fprintf(stderr, usage, argv[0]);
            exit(-1);
        }
    }
    if(inassemble(argv[optind]) == true) {
        exec();    /* プログラム実行 */
    }
    if(cerrno > 0) {
        freecerr();
        exit(-1);
    }
    return 0;
}
