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
 * @brief comet2コマンドのメイン
 *
 * @return 正常終了時は0、異常終了時は1
 *
 * @param argc コマンドライン引数の数
 * @param *argv[] コマンドライン引数の配列
 */
int main(int argc, char *argv[])
{
    int memsize = DEFAULT_MEMSIZE, clocks = DEFAULT_CLOCKS;
    int opt, stat = 0;
    const char *version = PACKAGE_VERSION,  *cmdversion = "comet2 of YACASL2 version %s\n";
    const char *usage = "Usage: %s [-vh] [-M <MEMORYSIZE>] [-C <CLOCKS>]\n";

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
            return 0;
        case '?':
            fprintf(stderr, usage, argv[0]);
            exit(1);
        }
    }
    /* エラーの定義 */
    cerr_init();
    addcerrlist_load();
    addcerrlist_exec();

    /* COMET II仮想マシンのリセット */
    reset(memsize, clocks);
    execptr->start = 0;
    execmode.step = true;
    exec();                /* プログラム実行 */
    shutdown();
    stat = (cerr->num == 0) ? 0 : 1;
    freecerr();                 /* エラーの解放 */
    return stat;
}
