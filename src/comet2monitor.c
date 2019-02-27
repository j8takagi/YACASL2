#include "package.h"
#include "exec.h"

/**
 * comet2monitorコマンドのオプション
 */
static struct option longopts[] = {
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
    const char *usage = "Usage: %s [-vh] [-M <MEMORYSIZE>] [-C <CLOCKS>]\n";

    /* エラーの定義 */
    cerr_init();
    addcerrlist_load();
    addcerrlist_exec();
    addcerrlist_comet2monitor();

    /* オプションの処理 */
    while((opt = getopt_long(argc, argv, "M:C:vh", longopts, NULL)) != -1) {
        switch(opt) {
        case 'M':
            memsize = atoi(optarg);
            break;
        case 'C':
            clocks = atoi(optarg);
            break;
        case 'v':
            fprintf(stdout, cmdversion, version);
            return 0;
        case 'h':
            fprintf(stdout, usage, argv[0]);
            goto comet2monitorfin;
        case '?':
            fprintf(stderr, usage, argv[0]);
            setcerr(212, "");    /* invalid option */
            goto comet2monitorfin;
        }
    }
    create_cmdtable(HASH_CMDTYPE);
    reset(memsize, clocks);     /* COMET II仮想マシンのリセット */
    execptr->start = 0;
    execmode.monitor = true;
    exec();                     /* プログラム実行 */
    shutdown();
comet2monitorfin:
    free_cmdtable(HASH_CMDTYPE);
    free_cmdtable(HASH_CODE);
    if(cerr->num > 0) {
        stat = 1;
    }
    freecerr();                 /* エラーの解放 */
    return stat;
}
