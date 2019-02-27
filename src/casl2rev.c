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
    int opt = 0;
    int stat = 0;
    const char *version = PACKAGE_VERSION;
    const char *cmdversion = "disassemble of YACASL2 version %s\n";
    const char *usage = "Usage: %s [-vh] FILE\n";

    /* エラーの定義 */
    cerr_init();
    addcerrlist_load();
    addcerrlist_exec();

    /* オプションの処理 */
    while((opt = getopt_long(argc, argv, "vh", longopts, NULL)) != -1) {
        switch(opt) {
        case 'v':
            fprintf(stdout, cmdversion, version);
            goto casl2revfin;
        case 'h':
            fprintf(stdout, usage, argv[0]);
            goto casl2revfin;
        case '?':
            fprintf(stderr, usage, argv[0]);
            setcerr(212, "");    /* invalid option */
            goto casl2revfin;
        }
    }
    if(argv[optind] == NULL) {
        setcerr(211, "");    /* object file not specified */
        fprintf(stderr, "disassemble error - %d: %s\n", cerr->num, cerr->msg);
        goto casl2revfin;
    }
    disassemble_file(argv[optind]);                /* プログラム実行 */
casl2revfin:
    if(cerr->num > 0) {
        stat = 1;
    }
    freecerr();                 /* エラーの解放 */
    return stat;
}
