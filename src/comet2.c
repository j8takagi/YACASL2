#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <getopt.h>

#include "exec.h"
#include "cmem.h"
#include "cerr.h"

/**
 * comet2コマンドのオプション
 */
static struct option longopts[] = {
    {"trace", no_argument, NULL, 't'},
    {"tracearithmetic", no_argument, NULL, 't'},
    {"tracelogical", no_argument, NULL, 'T'},
    {"dump", no_argument, NULL, 'd'},
    {"memorysize", required_argument, NULL, 'M'},
    {"clocks", required_argument, NULL, 'C'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

/**
 * comet2コマンドのメイン
 */
int main(int argc, char *argv[])
{
    int memsize = DEFAULT_MEMSIZE, clocks = DEFAULT_CLOCKS;
    int opt, stat = 0;
    const char *usage = "Usage: %s [-tTdh] [-M <MEMORYSIZE>] [-C <CLOCKS>] FILE\n";

    cerr_init();
    addcerrlist_load();
    addcerrlist_exec();

    /* オプションの処理 */
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
            exit(1);
        }
    }
    if(argv[optind] == NULL) {
        setcerr(211, "");    /* object file not specified */
        fprintf(stderr, "comet2 error - %d: %s\n", cerr->num, cerr->msg);
        exit(1);
    }
    /* COMET II仮想マシンのリセット */
    reset(memsize, clocks);
    execptr->start = 0;
    if(loadassemble(argv[optind]) == true) {
        exec();                /* プログラム実行 */
    }
    /* COMET II仮想マシンのシャットダウン */
    shutdown();
    stat = (cerr->num == 0) ? 0 : -1;
    /* エラーの解放 */
    freecerr();
    return stat;
}
