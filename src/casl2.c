#include <stdio.h>
#include <stdlib.h>

#define _GNU_SOURCE
#include <getopt.h>

#include "cmem.h"
#include "cerr.h"
#include "assemble.h"
#include "exec.h"

/**
 * casl2コマンドのオプション
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
    { "memorysize", required_argument, NULL, 'M' },
    { "clocks", required_argument, NULL, 'C' },
    { "help", no_argument, NULL, 'h' },
    { 0, 0, 0, 0 },
};

/**
 * casl2のエラー定義
 */
CERR cerr_casl2[] = {
    { 126, "no source file" },
};

/**
 * CASL IIのエラーをエラーリストに追加
 */
void addcerrlist_casl2()
{
    addcerrlist(ARRAYSIZE(cerr_casl2), cerr_casl2);
}

/**
 * アセンブル結果を書き込むファイルの名前
 */
const char *objfile_name(const char *str)
{
    const char *default_name = "a.o";
    return (str == NULL) ? default_name : str;
}

/**
 * 指定された1つまたは複数のファイルを2回アセンブル
 */
void assemble(int filec, char *filev[])
{
    int i;
    PASS pass;
    WORD bp[filec];

    create_cmdtype_code();                         /* 命令の名前とタイプがキーのハッシュ表を作成 */
    asptr = malloc_chk(sizeof(ASPTR), "asptr");    /* アセンブル時のプロパティ用の領域確保 */
    asptr->prog = malloc_chk(sizeof(LABELSIZE + 1), "asptr.prog");
    asptr->ptr = 0;
    /* アセンブル。ラベル表作成のため、2回行う */
    for(pass = FIRST; pass <= SECOND; pass++) {
        for(i = 0; i < filec; i++) {
            /* データの格納開始位置 */
            if(pass == FIRST) {
                bp[i] = asptr->ptr;
            } else if(pass == SECOND) {
                asptr->ptr = bp[i];
            }
            if(execmode.trace == true || execmode.dump == true ||
               asmode.src == true || asmode.label == true || asmode.asdetail == true)
            {
                fprintf(stdout, "\nAssemble %s (%d)\n", filev[i], pass);
            }
            /* ファイルをアセンブル */
            if(assemblefile(filev[i], pass) == false) {
                goto asfin;
            }
        }
        if(pass == FIRST && asmode.label == true) {
            fprintf(stdout, "\nLabel::::\n");
            printlabel();
            if(asmode.onlylabel == true) {
                break;
            }
        }
    }
asfin:
    freelabel();                                  /* ラベルハッシュ表を解放 */
    free_cmdtype_code();                          /* 命令の名前とタイプがキーのハッシュ表を解放 */
    FREE(asptr->prog);                            /* アセンブル時のプロパティを解放 */
    FREE(asptr);
}

/**
 *  casl2コマンドのメイン
 */
int main(int argc, char *argv[])
{
    int memsize = DEFAULT_MEMSIZE, clocks = DEFAULT_CLOCKS, opt, i, stat;
    char *af[argc];
    char *objfile = NULL;
    const char *usage =
        "Usage: %s [-slLaAtTdh] [-oO[<OBJECTFILE>]] [-M <MEMORYSIZE>] [-C <CLOCKS>] FILE1[ FILE2  ...]\n";

    cerr_init();
    addcerrlist_casl2();
    addcerrlist_assemble();
    addcerrlist_exec();
    /* オプションの処理 */
    while((opt = getopt_long(argc, argv, "tTdslLao::O::AM:C:h", longopts, NULL)) != -1) {
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
            objfile = strdup_chk(objfile_name(optarg), "objfile");
            break;
        case 'O':
            asmode.onlyassemble = true;
            objfile = strdup_chk(objfile_name(optarg), "objfile");
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
    /* ソースファイルが指定されていない場合は終了 */
    if(argv[optind] == NULL) {
        setcerr(126, "");    /* no source file */
        fprintf(stderr, "CASL2 error - %d: %s\n", cerr->num, cerr->msg);
        exit(1);
    }
    reset(memsize, clocks);                        /* 仮想マシンCOMET IIのリセット */
    for(i = 0; i < argc - optind; i++) {           /* 引数からファイル名配列を取得 */
        af[i] = argv[optind + i];
    }
    assemble(i, af);                               /* アセンブル */
    if(asmode.onlylabel == true || cerr->num > 0) {
        goto casl2fin;
    }
    /* オブジェクトファイル名が指定されている場合は、アセンブル結果をオブジェクトファイルに出力 */
    if(objfile != NULL) {
        outassemble(objfile);
        FREE(objfile);
    }
    /* onlyassembleモード以外の場合、仮想マシンCOMET IIを実行 */
    if(asmode.onlyassemble == false) {
        exec();                                    /* 仮想マシンCOMET IIの実行 */
    }
casl2fin:
    shutdown();                                    /* 仮想マシンCOMET IIのシャットダウン */
    stat = (cerr->num == 0) ? 0 : 1;
    freecerr();                                    /* エラーの解放 */
    return stat;
}
