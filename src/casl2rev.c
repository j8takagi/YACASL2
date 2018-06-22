#include "package.h"
#include "exec.h"

/**
 * @brief casl2revコマンドのオプション
 */
static struct option longopts[] = {
    {"version", no_argument, NULL, 'v' },
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

/**
 * @brief casl2revコマンドのメイン
 *
 * @return 正常終了時は0、異常終了時は1
 *
 * @param argc コマンドライン引数の数
 * @param *argv[] コマンドライン引数の配列
 */
int main(int argc, char *argv[])
{
    int opt, stat = 0;
    const char *version = PACKAGE_VERSION,  *cmdversion = "disassemble of YACASL2 version %s\n";
    const char *usage = "Usage: %s [-vh] FILE\n";

    cerr_init();
    addcerrlist_load();
    addcerrlist_exec();

    /* オプションの処理 */
    while((opt = getopt_long(argc, argv, "vh", longopts, NULL)) != -1) {
        switch(opt) {
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
    if(argv[optind] == NULL) {
        setcerr(211, "");    /* object file not specified */
        fprintf(stderr, "disassemble error - %d: %s\n", cerr->num, cerr->msg);
        exit(1);
    }
    disassemble_file(argv[optind]);                /* プログラム実行 */
    stat = (cerr->num == 0) ? 0 : 1;
    /* エラーの解放 */
    freecerr();
    return stat;
}
