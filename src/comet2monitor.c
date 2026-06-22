#define _GNU_SOURCE
#include "package.h"
#include "exec.h"
#include "load.h"
#include "monitor.h"

/**
 * comet2monitorコマンドのオプション
 */
static struct option longopts[] = {
    {"trace", no_argument, NULL, 't'},
    {"tracearithmetic", no_argument, NULL, 't'},
    {"tracelogical", no_argument, NULL, 'T'},
    {"dump", no_argument, NULL, 'd'},
    {"memorysize", required_argument, NULL, 'M'},
    {"clocks", required_argument, NULL, 'C'},
    {"version", no_argument, NULL, 'v' },
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

/**
 * @brief casl2のエラー定義
 */
CERR cerr_comet2monitor[] = {
    { 401, "invalid option" },
};

void addcerrlist_comet2monitor()
{
    addcerrlist(ARRAYSIZE(cerr_comet2monitor), cerr_comet2monitor);
}

/**
 * @brief comet2monitorコマンドのメイン
 *
 * @return 正常終了時は0、異常終了時は1
 *
 * @param argc コマンドライン引数の数
 * @param *argv[] コマンドライン引数の配列
 */
int main(int argc, char *argv[])
{
    int memsize = DEFAULT_MEMSIZE;
    int clocks = DEFAULT_CLOCKS;
    int opt = 0;
    int stat = 0;
    const char *version = PACKAGE_VERSION;
    const char *cmdversion = "comet2monitor: COMET II machine code monitor of YACASL2 version %s\n";
    const char *usage = "Usage: %s [-tTdvh] [-M <MEMORYSIZE>] [-C <CLOCKS>] FILE\n";

    /* エラーの定義 */
    cerr_init();
    addcerrlist_load();
    addcerrlist_exec();
    addcerrlist_comet2monitor();

    /* オプションの処理 */
    while((opt = getopt_long(argc, argv, "tTdM:C:vh", longopts, NULL)) != -1) {
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
            if((memsize = memsize_str2word(optarg)) == 0) {
                goto comet2monitorfin;
            }
            break;
        case 'C':
            if((clocks = clock_str2clock(optarg)) == 0) {
                goto comet2monitorfin;
            }
            break;
        case 'v':
            fprintf(stdout, cmdversion, version);
            return 0;
        case 'h':
            fprintf(stdout, usage, argv[0]);
            return 0;
        case '?':
            fprintf(stderr, usage, argv[0]);
            setcerr(212, "");    /* invalid option */
            goto comet2monitorfin;
        }
    }
    create_cmdtable(HASH_CMDTYPE);
    comet2_init(memsize, clocks);     /* COMET II仮想マシンの初期化 */
    execptr->start = 0;
    if(argv[optind] != NULL) {
        execptr->end = loadassemble(argv[optind++], execptr->start);
    }
    /* 残りの引数（オプション以外の引数）があるかチェック */
    if (optind < argc) {
        warn_ignore_arg(argc - optind, argv + optind);
    }
    execmode.monitor = true;
    exec();                     /* プログラム実行 */
    comet2_shutdown();
comet2monitorfin:
    free_cmdtable(HASH_CMDTYPE);
    if(cerr->num > 0) {
        stat = 1;
    }
    freecerr();                 /* エラーの解放 */
    return stat;
}
