#include "package.h"
#include "word.h"

/**
 * @brief dumpwordコマンドのオプション
 */
static struct option longopts[] = {
    { "arithmetic", no_argument, NULL, 'a' },
    { "logical", no_argument, NULL, 'l' },
    { "version", no_argument, NULL, 'v' },
    { "help", no_argument, NULL, 'h' },
    { 0, 0, 0, 0 },
};

/**
 * @brief dumpwordコマンドのメイン
 *
 * @return 正常終了時は0、エラー発生時は1
 *
 * @param argc コマンドライン引数の数
 * @param *argv[] コマンドライン引数の配列
 */
int main(int argc, char *argv[])
{
    bool logicalmode = false;    /* レジストリの内容を論理値（0から65535）で表示する場合はtrue */
    int opt, stat = 0;
    WORD word;
    const char *version = PACKAGE_VERSION,  *cmdversion = "dumpword of YACASL2 version %s\n";
    const char *usage = "Usage: %s [-alh] WORD\n";

    /* エラーの定義 */
    cerr_init();
    addcerrlist_load();
    addcerrlist_word();

    /* オプションの処理 */
    while((opt = getopt_long(argc, argv, "alvh", longopts, NULL)) != -1) {
        switch(opt) {
        case 'l':
            logicalmode = true;
            break;
        case 'v':
            fprintf(stdout, cmdversion, version);
            goto dumpwordfin;
        case 'h':
            fprintf(stdout, usage, argv[0]);
            goto dumpwordfin;
        case '?':
            fprintf(stderr, usage, argv[0]);
            setcerr(212, "");    /* invalid option */
            goto dumpwordfin;
        }
    }

    if(argv[optind] == NULL) {
        setcerr(213, "");    /* invalid argument */
        fprintf(stderr, usage, argv[0]);
        goto dumpwordfin;
    }
    /* WORD値に変換 */
    word = nh2word(argv[optind]);
    if(cerr->num > 0) {
        fprintf(stderr, "Dumpword Error - %d: %s\n", cerr->num, cerr->msg);
        goto dumpwordfin;
    }
    fprintf(stdout, "%6s: ", argv[optind]);
    print_dumpword(word, logicalmode);
    fprintf(stdout, "\n");
dumpwordfin:
    if(cerr->num > 0) {
        stat = 1;
    }
    freecerr();                 /* エラーの解放 */
    return stat;
}
