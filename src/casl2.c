#include "casl2.h"
#include "assemble.h"
#include "exec.h"
#define _GNU_SOURCE
#include <getopt.h>

/* casl2コマンドのオプション */
static struct option longopts[] = {
    {"source", no_argument, NULL, 's'},
    {"label", no_argument, NULL, 'l'},
    {"labelonly", no_argument, NULL, 'L'},
    {"assembledetail", no_argument, NULL, 'a'},
    {"assembledetailonly", no_argument, NULL, 'A'},
    {"assembleout", optional_argument, NULL, 'o'},
    {"assembleoutonly", optional_argument, NULL, 'O'},
    {"trace", no_argument, NULL, 't'},
    {"tracearithmetic", no_argument, NULL, 't'},
    {"tracelogical", no_argument, NULL, 'T'},
    {"dump", no_argument, NULL, 'd'},
    {"memorysize", required_argument, NULL, 'M'},
    {"clocks", required_argument, NULL, 'C'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

/* casl2のエラー定義 */
CERRARRAY cerr_casl2[] = {
    { 126, "source file is not specified" },
};
bool addcerrlist_casl2()
{
    return addcerrlist(sizeof(cerr_casl2), cerr_casl2);
}

/* 指定されたファイルにアセンブル結果を書込 */
void outassemble(const char *file) {
    FILE *fp;
    if((fp = fopen(file, "w")) == NULL) {
        perror(file);
        exit(-1);
    }
    fwrite(memory, sizeof(WORD), endptr, fp);
    fclose(fp);
}

/* アセンブル結果を書き込むファイルの名前 */
const char *objfile_name(const char *str)
{
    const char *default_name = "a.o";

    if(str == NULL) {
        return default_name;
    } else {
        return str;
    }
}

/* casl2コマンドのメイン */
int main(int argc, char *argv[])
{
    int opt, i;
    PASS pass;
    bool status = false;
    WORD beginptr[argc];
    char *objfile = NULL;
    const char *usage =
        "Usage: %s [-slLaAtTdh] [-oO<OBJECTFILE>] [-M <MEMORYSIZE>] [-C <CLOCKS>] FILE ...\n";

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
            objfile = strdup(objfile_name(optarg));
            break;
        case 'O':
            asmode.onlyassemble = true;
            objfile = strdup(objfile_name(optarg));
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
            exit(-1);
        }
    }

    addcerrlist_casl2();
    /* ソースファイルが指定されていない場合は終了 */
    if(argv[optind] == NULL) {
        setcerr(126, NULL);    /* source file is not specified */
        fprintf(stderr, "CASL2 error - %d: %s\n", cerrno, cerrmsg);
        goto casl2err;
    }
    /* COMET II仮想マシンのリセット */
    reset();
    /* アセンブル。ラベル表作成のため、2回行う */
    for(pass = FIRST; pass <= SECOND; pass++) {
        if(pass == FIRST && create_cmdtype_code() == false) {
            goto casl2err;
        }
        for(i = optind; i < argc; i++) {
            /* データの格納開始位置 */
            if(pass == FIRST) {
                beginptr[i] = ptr;
            } else if(pass == SECOND) {
                ptr = beginptr[i];
            }
            if(execmode.trace == true || execmode.dump == true || asmode.src == true ||
               asmode.label == true || asmode.asdetail == true)
            {
                fprintf(stdout, "\nAssemble %s (%d)\n", argv[i], pass);
            }
            if((status = assemble(argv[i], pass)) == false) {
                goto casl2err;
            }
        }
        if(pass == FIRST && asmode.label == true) {
            fprintf(stdout, "\nLabel::::\n");
            printlabel();
            if(asmode.onlylabel == true) {
                return 0;
            }
        }
    }
    free_cmdtype_code();    /* 命令表の解放 */
    freelabel();            /* ラベル表の解放 */
    if(status == true) {
        if(objfile != NULL) {
            outassemble(objfile);
        }
        if(asmode.onlyassemble == false) {
            exec();    /* プログラム実行 */
        }
    }
    shutdown();
    if(cerrno > 0) {
        goto casl2err;
    }
    return 0;
casl2err:
    freecerr();
    exit(-1);
}
