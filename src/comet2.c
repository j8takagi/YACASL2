#include "casl2.h"
#include "exec.h"
#define _GNU_SOURCE
#include <getopt.h>

/* comet2コマンドのオプション */
static struct option longopts[] = {
    {"trace", no_argument, NULL, 't'},
    {"tracearithmetic", no_argument, NULL, 't'},
    {"tracelogical", no_argument, NULL, 'T'},
    {"dump", no_argument, NULL, 'd'},
    {"memorysize", required_argument, NULL, 'M'},
    {"clocks", required_argument, NULL, 'C'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0}
};

/* エラー番号とエラーメッセージ */
CERRARRAY cerr_comet2[] = {
    { 201, "Load object file - full of COMET II memory" },
    { 202, "SVC input - out of Input memory" },
    { 203, "SVC output - out of COMET II memory" },
    { 204, "Program Register (PR) - out of COMET II memory" },
    { 205, "Stack Pointer (SP) - cannot allocate stack buffer" },
    { 206, "Address - out of COMET II memory" },
    { 207, "Stack Pointer (SP) - out of COMET II memory" },
};

/* 指定されたファイルからアセンブル結果を読込 */
bool loadassemble(char *file) {
    FILE *fp;
    if((fp = fopen(file, "r")) == NULL) {
        perror(file);
        return false;
    }
    if((endptr = startptr + fread(memory, sizeof(WORD), memsize-startptr, fp)) == memsize) {
        setcerr(201, NULL);    /* Load object file - full of COMET II memory */
        fprintf(stderr, "Execute error - %d: %s\n", cerrno, cerrmsg);
        return false;
    }
    fclose(fp);
    return true;
}

/* comet2コマンド */
int main(int argc, char *argv[])
{
    int opt;
    const char *usage = "Usage: %s [-tTdh] [-M <memorysize>] [-C <clocks>] FILE\n";

    while((opt = getopt_long(argc, argv, "tTdM:C:h", longopts, NULL)) != -1) {
        switch(opt) {
        case 't':
            execmode.trace = true;
            break;
        case 'T':
            execmode.trace = true;
            execmode.logical = true;
            break;
        case 'd':
            execmode.dump = true;
            break;
        case 'M':
            memsize = atoi(optarg);
            break;
        case 'C':
            clocks = atoi(optarg);
            break;
        case 'h':
            fprintf(stdout, usage, argv[0]);
            return 0;
        case '?':
            fprintf(stderr, usage, argv[0]);
            exit(-1);
        }
    }
    /* エラーリストにerr_comet2を追加 */
    addcerrlist(ARRAYSIZE(cerr_comet2), cerr_comet2);
    reset();
    startptr = 0;
    if(loadassemble(argv[optind]) == true) {
        exec();    /* プログラム実行 */
    }
    if(cerrno > 0) {
        freecerr();
        exit(-1);
    }
    return 0;
}
