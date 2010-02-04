#include "casl2.h"
#include "assemble.h"
#include "exec.h"
#define _GNU_SOURCE
#include <getopt.h>

static struct option longopts[] =
{
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

ASMODE asmode = {false, false, false, false, false};
EXECMODE execmode = {false, false, false};

/* 指定されたファイルにCOMET II仮想メモリ（アセンブル結果）を書込 */
void outassemble(char *file) {
    FILE *fp;
    if((fp = fopen(file, "w")) == NULL) {
        perror(file);
        exit(-1);
    }
    fwrite(memory, sizeof(WORD), endptr, fp);
    fclose(fp);
}

int main(int argc, char *argv[])
{
    int opt, i;
    PASS pass;
    bool status = false;
    WORD beginptr[argc];
    char *objfile = NULL;
    const char *default_objfile = "a.o";
    const char *usage =
        "Usage: %s [-slLaAtTdh] [-oO<OUTFILE>] [-M <memorysize>] [-C <clocks>] FILE ...\n";

    while((opt = getopt_long(argc, argv, "tTdslLao::O::AM:C:h", longopts, NULL)) != -1) {
        switch(opt) {
        case 's':
            (&asmode)->srcmode = true;
            break;
        case 'l':
            (&asmode)->labelmode = true;
            break;
        case 'L':
            (&asmode)->labelmode = true;
            (&asmode)->onlylabelmode = true;
            break;
        case 'a':
            (&asmode)->asdetailmode = true;
            break;
        case 'A':
            (&asmode)->onlyassemblemode = true;
            (&asmode)->asdetailmode = true;
            break;
        case 'o':
            if(optarg == NULL) {
                objfile = strdup(default_objfile);
            } else {
                objfile = strdup(optarg);
            }
            break;
        case 'O':
            (&asmode)->onlyassemblemode = true;
            if(optarg == NULL) {
                objfile = strdup(default_objfile);
            } else {
                objfile = strdup(optarg);
            }
            break;
        case 't':
            (&execmode)->tracemode = true;
            break;
        case 'T':
            (&execmode)->tracemode = true;
            (&execmode)->logicalmode = true;
            break;
        case 'd':
            (&execmode)->dumpmode = true;
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
        fprintf(stderr, "source file is not specified\n");
        exit(-1);
    }
    /* ソースファイルが指定されていない場合は終了 */
    reset();
    /* アセンブル。ラベル表作成のため、2回行う */
    for(pass = FIRST; pass <= SECOND; pass++) {
        for(i = optind; i < argc; i++) {
            /* データの格納開始位置 */
            if(pass == FIRST) {
                beginptr[i] = ptr;
            } else if(pass == SECOND) {
                ptr = beginptr[i];
            }
            if((&execmode)->tracemode == true || (&execmode)->dumpmode == true ||
               (&asmode)->srcmode == true || (&asmode)->labelmode == true ||
               (&asmode)->asdetailmode == true)
            {
                fprintf(stdout, "\nAssemble %s (%d)\n", argv[i], pass);
            }
            if((status = assemble(argv[i], pass)) == false) {
                freelabel();    /* ラベル表の解放 */
                if(cerrno > 0) {
                    freecerr();    /* エラーの解放 */
                }
                exit(-1);
            }
        }
        if(pass == FIRST && (&asmode)->labelmode == true) {
            fprintf(stdout, "\nLabel::::\n");
            printlabel();
            if((&asmode)->onlylabelmode == true) {
                return 0;
            }
        }
    }
    freelabel();    /* ラベル表の解放 */
    if(status == true) {
        if(objfile != NULL) {
            outassemble(objfile);
        }
        if((&asmode)->onlyassemblemode == false) {
            exec();    /* プログラム実行 */
        }
    }
    if(cerrno > 0) {
        freecerr();
        exit(-1);
    }
    return 0;
}
