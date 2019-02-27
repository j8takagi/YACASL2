#include "package.h"
#include "exec.h"

/**
 * comet2コマンドのオプション
 */
static struct option longopts[] = {
    {"trace", no_argument, NULL, 't'},
    {"tracearithmetic", no_argument, NULL, 't'},
    {"tracelogical", no_argument, NULL, 'T'},
    {"dump", no_argument, NULL, 'd'},
    {"monitor", no_argument, NULL, 'm'},
    {"memorysize", required_argument, NULL, 'M'},
    {"clocks", required_argument, NULL, 'C'},
    {"version", no_argument, NULL, 'v' },
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};


/**
 * @brief comet2コマンドのメイン
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
    const char *cmdversion = "comet2 of YACASL2 version %s\n";
    const char *usage = "Usage: %s [-tTdmvh] [-M <MEMORYSIZE>] [-C <CLOCKS>] FILE\n";

    /* エラーの定義 */
    cerr_init();
    addcerrlist_load();
    addcerrlist_exec();

    /* オプションの処理 */
    while((opt = getopt_long(argc, argv, "tTdmM:C:vh", longopts, NULL)) != -1) {
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
        case 'm':
            execmode.monitor = true;
            break;
        case 'M':
            memsize = atoi(optarg);
            break;
        case 'C':
            clocks = atoi(optarg);
            break;
        case 'v':
            fprintf(stdout, cmdversion, version);
            goto comet2fin;
        case 'h':
            fprintf(stdout, usage, argv[0]);
            goto comet2fin;
        case '?':
            fprintf(stderr, usage, argv[0]);
            setcerr(212, "");    /* invalid option */
            goto comet2fin;
        }
    }
    if(argv[optind] == NULL) {
        setcerr(211, "");    /* object file not specified */
        fprintf(stderr, "comet2 error - %d: %s\n", cerr->num, cerr->msg);
        goto comet2fin;
    }
    reset(memsize, clocks);     /* COMET II仮想マシンのリセット */
    execptr->start = 0;
    execptr->end = loadassemble(argv[optind], execptr->start);
    if(execptr->end > 0 && cerr->num == 0) {
        exec();                 /* プログラム実行 */
    }
    shutdown();                 /* COMET II仮想マシンのシャットダウン */
comet2fin:
    if(cerr->num > 0) {
        stat = 1;
    }
    freecerr();                 /* エラーの解放 */
    return stat;
}
