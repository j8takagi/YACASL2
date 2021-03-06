#include "package.h"
#include "assemble.h"
#include "exec.h"

/**
 * @brief CASL IIのエラーをエラーリストに追加
 *
 * @return なし
 */
void addcerrlist_casl2();

/**
 * @brief アセンブル結果を書き込むファイルの名前
 *
 * @return ファイル名
 *
 * @param *str ファイル名
 */
char *objfile_name(const char *str);

/**
 * @brief casl2コマンドのオプション
 */
static struct option longopts[] = {
    { "source", no_argument, NULL, 's' },
    { "label", no_argument, NULL, 'l' },
    { "labelonly", no_argument, NULL, 'L' },
    { "assembledetail", no_argument, NULL, 'a' },
    { "assembledetailonly", no_argument, NULL, 'A' },
    { "assembleout", optional_argument, NULL, 'o' },
    { "assembleoutonly", optional_argument, NULL, 'O' },
    { "trace", no_argument, NULL, 't' },
    { "tracearithmetic", no_argument, NULL, 't' },
    { "tracelogical", no_argument, NULL, 'T' },
    { "dump", no_argument, NULL, 'd' },
    { "monitor", no_argument, NULL, 'm' },
    { "memorysize", required_argument, NULL, 'M' },
    { "clocks", required_argument, NULL, 'C' },
    { "version", no_argument, NULL, 'v' },
    { "help", no_argument, NULL, 'h' },
    { 0, 0, 0, 0 },
};

/**
 * @brief casl2のエラー定義
 */
CERR cerr_casl2[] = {
    { 126, "no source file" },
    { 127, "invalid option" },
};

void addcerrlist_casl2()
{
    addcerrlist(ARRAYSIZE(cerr_casl2), cerr_casl2);
}

char *objfile_name(const char *name)
{
    const char *default_name = "a.o";
    return strdup_chk(
        (name == NULL || !name[0]) ? default_name : name,
        "objfile_name"
        );
}

/**
 * @brief casl2コマンドのメイン
 *
 * @return 正常終了時は0、エラー発生時は1
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
    int asfilecnt = 0;
    char **asfile = NULL;
    char *objfile = NULL;
    const char *version = PACKAGE_VERSION;
    const char *cmdversion = "casl2 of YACASL2 version %s\n";
    const char *usage =
        "Usage: %s [-slLaAtTdmvh] [-oO[<OBJECTFILE>]] [-M <MEMORYSIZE>] [-C <CLOCKS>] FILE1[ FILE2  ...]\n";

    /* エラーの定義 */
    cerr_init();
    addcerrlist_casl2();
    addcerrlist_assemble();
    addcerrlist_exec();

    /* オプションの処理 */
    while((opt = getopt_long(argc, argv, "tTdslLmao::O::AM:C:vh", longopts, NULL)) != -1) {
        switch(opt) {
        case 's':
            asmode.src = true;
            break;
        case 'l':
            asmode.label = true;
            break;
        case 'L':
            asmode.label = true;
            asmode.onlylabel = true;
            break;
        case 'a':
            asmode.asdetail = true;
            break;
        case 'A':
            asmode.asdetail = true;
            asmode.onlyassemble = true;
            break;
        case 'o':
            objfile = objfile_name(optarg);
            break;
        case 'O':
            asmode.onlyassemble = true;
            objfile = objfile_name(optarg);
            break;
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
            execmode.step = true;
            break;
        case 'M':
            memsize = atoi(optarg);
            break;
        case 'C':
            clocks = atoi(optarg);
            break;
        case 'v':
            fprintf(stdout, cmdversion, version);
            goto casl2fin;
        case 'h':
            fprintf(stdout, usage, argv[0]);
            goto casl2fin;
        case '?':
            fprintf(stderr, usage, argv[0]);
            setcerr(212, "");    /* invalid option */
            goto casl2fin;
        }
    }

    /* ソースファイルが指定されていない場合は終了 */
    if(argv[optind] == NULL) {
        setcerr(126, "");    /* no source file */
        fprintf(stderr, "casl2 error - %d: %s\n", cerr->num, cerr->msg);
        goto casl2fin;
    }
    create_cmdtable(HASH_CMDTYPE);                 /* 命令の名前とタイプがキーのハッシュ表を作成 */
    reset(memsize, clocks);                        /* 仮想マシンCOMET IIのリセット */
    asfilecnt = argc - optind;
    asfile = calloc_chk(asfilecnt, sizeof(char *), "asfile");
    for(int i = 0; i < asfilecnt; i++) {           /* 引数からファイル名配列を取得 */
        asfile[i] = argv[optind + i];
    }
    /* アセンブル */
    if(assemble(asfilecnt, asfile, 0) == false || asmode.onlylabel == true) {
        goto shutdown;
    }
    /* オブジェクトファイル名が指定されている場合は、アセンブル結果をオブジェクトファイルに出力 */
    if(objfile != NULL) {
        outassemble(objfile);
    }
    /* onlyassembleモード以外の場合、仮想マシンCOMET IIを実行 */
    if(asmode.onlyassemble == false) {
        exec();                                    /* 仮想マシンCOMET IIの実行 */
    }
shutdown:
    shutdown();                                   /* 仮想マシンCOMET IIのシャットダウン */
casl2fin:
    FREE(objfile);
    FREE(asfile);
    free_cmdtable(HASH_CMDTYPE);
    if(cerr->num > 0) {
        stat = 1;
    }
    freecerr();                                    /* エラーの解放 */
    return stat;
}
