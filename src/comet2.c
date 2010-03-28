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

/* comet2のエラー定義 */
CERR cerr_comet2[] = {
    { 201, "load object file - full of COMET II memory" },
    { 208, "object file is not specified" },
};
bool addcerrlist_comet2()
{
    return addcerrlist(ARRAYSIZE(cerr_comet2), cerr_comet2);
}

/* 指定されたファイルからアセンブル結果を読込 */
bool loadassemble(char *file) {
    FILE *fp;
    bool status = true;

    if((fp = fopen(file, "r")) == NULL) {
        perror(file);
        return false;
    }
    prog->end = prog->start +
        fread(sys->memory, sizeof(WORD), sys->memsize - prog->start, fp);
    if(prog->end == sys->memsize) {
        setcerr(201, NULL);    /* Load object file - full of COMET II memory */
        fprintf(stderr, "Execute error - %d: %s\n", cerr->num, cerr->msg);
        status = false;
    }
    fclose(fp);
    return status;
}

/* comet2コマンド */
int main(int argc, char *argv[])
{
    int memsize = DEFAULT_MEMSIZE, clocks = DEFAULT_CLOCKS;
    int opt, status = 0;
    const char *usage = "Usage: %s [-tTdh] [-M <MEMORYSIZE>] [-C <CLOCKS>] FILE\n";

    cerr_init();
    addcerrlist_comet2();
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
            exit(-1);
        }
    }
    if(argv[optind] == NULL) {
        setcerr(208, NULL);    /* object file is not specified */
        fprintf(stderr, "comet2 error - %d: %s\n", cerr->num, cerr->msg);
        exit(-1);
    }
    /* COMET II仮想マシンのリセット */
    reset(memsize, clocks);
    prog->start = 0;
    if(loadassemble(argv[optind]) == true) {
        create_code_type();    /* 命令と命令タイプがキーのハッシュ表を作成 */
        exec();                /* プログラム実行 */
        free_code_type();      /* 命令と命令タイプがキーのハッシュ表を解放 */
    }
    /* COMET II仮想マシンのシャットダウン */
    shutdown();
    if(cerr->num > 0) {
        status = -1;
    }
    /* エラーの解放 */
    freecerr();
    return status;
}
