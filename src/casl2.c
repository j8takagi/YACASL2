#include "casl2.h"
#include "assemble.h"
#include "exec.h"
#define _GNU_SOURCE
#include <getopt.h>

/* 指定されたファイルにCOMET II仮想メモリ（アセンブル結果）を書込 */
void outassemble(char *file) {
    FILE *fp;
    if((fp = fopen(file, "w")) == NULL) {
        perror(file);
        return;
    }
    fwrite(memory, sizeof(WORD), endptr, fp);
    fclose(fp);
}

static struct option longopts[] = {
    {"trace", no_argument, NULL, 't'},
    {"tracearithmetic", no_argument, NULL, 't'},
    {"tracelogical", no_argument, NULL, 'T'},
    {"dump", no_argument, NULL, 'd'},
    {"source", no_argument, NULL, 's'},
    {"label", no_argument, NULL, 'l'},
    {"assembledetail", no_argument, NULL, 'a'},    
    {"onlyassemble", optional_argument, NULL, 'o'},
    {"assembledetailonly", no_argument, NULL, 'A'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0}
};

int main(int argc, char *argv[])
{
    int opt, i;
    PASS pass;
    bool status = false;
    WORD beginptr[argc];
    char *objfile = NULL;
    const char *usage = "Usage: %s [-tTdslaAh] [-o <OUTFILE>] FILE ...\n";

    while((opt = getopt_long(argc, argv, "tTdslao:Ah", longopts, NULL)) != -1) {
        switch(opt) {
        case 't':
            tracemode = true;
            break;
        case 'T':
            tracemode = true;
            logicalmode = true;
            break;
        case 'd':
            dumpmode = true;
            break;
        case 's':
            srcmode = true;
            break;
        case 'l':
            labelmode = true;
            break;
        case 'a':
            asdetailmode = true;
            break;
        case 'o':
            onlyassemblemode = true;
            if(optarg != NULL) {
                objfile = strdup(optarg);
            }
            break;
        case 'A':
            onlyassemblemode = true;
            objfile = NULL;
            asdetailmode = true;
            break;
        case 'h':
            fprintf(stdout, usage, argv[0]);
            exit(-1);
        case '?':
            fprintf(stderr, usage, argv[0]);
            exit(-1);
        }
    }
    /* アセンブル。ラベル表作成のため、2回行う */
    for(pass = FIRST; pass <= SECOND; pass++) {
        for(i = optind; i < argc; i++) {
            /* データの格納開始位置 */
            if(pass == FIRST) {
                beginptr[i] = ptr;
            } else if(pass == SECOND) {
                ptr = beginptr[i];
            }
            if(tracemode == true || dumpmode == true || srcmode == true ||
               labelmode == true || asdetailmode == true) {
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
        if(pass == FIRST && labelmode == true) {
            fprintf(stdout, "\nLabel::::\n");
            printlabel();
        }
    }
    freelabel();    /* ラベル表の解放 */
    if(status == true) {
        if(onlyassemblemode) {
            if(objfile != NULL) {
                outassemble(objfile);
            }
        } else {
            exec();    /* プログラム実行 */
        }
    }
    if(cerrno > 0) {
        freecerr();
        exit(-1);
    }
    return 0;
}
