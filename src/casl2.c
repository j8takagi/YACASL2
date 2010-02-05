#include "casl2.h"
#include "assemble.h"
#include "exec.h"
#define _GNU_SOURCE
#include <getopt.h>

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

ASMODE asmode = {false, false, false, false, false};
EXECMODE execmode = {false, false, false};

/* エラー番号とエラーメッセージ */
CERRARRAY cerr[] = {
    { 101, "label already defined" },
    { 102, "label table is full" },
    { 103, "label not found" },
    { 104, "label length is too long" },
    { 105, "no command in the line" },
    { 106, "operand count mismatch" },
    { 107, "no label in START" },
    { 108, "not command of operand \"r\"" },
    { 109, "not command of operand \"r1,r2\"" },
    { 110, "not command of operand \"r,adr[,x]\"" },
    { 111, "not command of operand \"adr[,x]\"" },
    { 112, "not command of no operand" },
    { 113, "command not defined" },
    { 114, "not integer" },
    { 115, "not hex" },
    { 116, "out of hex range" },
    { 117, "operand is too many" },
    { 118, "operand length is too long" },
    { 119, "out of COMET II memory" },
    { 120, "GR0 in operand x" },
    { 121, "cannot get operand token" },
    { 122, "cannot create hash table" },
    { 123, "illegal string" },
    { 124, "more than one character in literal" },
    { 201, "execute - out of COMET II memory" },
    { 202, "SVC input - out of Input memory" },
    { 203, "SVC output - out of COMET II memory" },
    { 204, "Program Register (PR) - out of COMET II memory" },
    { 205, "Stack Pointer (SP) - cannot allocate stack buffer" },
    { 206, "Address - out of COMET II memory" },
    { 207, "Stack Pointer (SP) - out of COMET II memory" },
    { 0, NULL },
};

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

    if(optarg == NULL) {
        return default_name;
    } else {
        return str;
    }
}

/* casl2コマンド */
int main(int argc, char *argv[])
{
    int opt, i;
    PASS pass;
    bool status = false;
    WORD beginptr[argc];
    char *objfile = NULL;
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
            (&asmode)->asdetailmode = true;
            (&asmode)->onlyassemblemode = true;
            break;
        case 'o':
            objfile = strdup(objfile_name(optarg));
            break;
        case 'O':
            (&asmode)->onlyassemblemode = true;
            objfile = strdup(objfile_name(optarg));
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
    /* ソースファイルが指定されていない場合は終了 */
    if(argv[optind] == NULL) {
        fprintf(stderr, "source file is not specified\n");
        exit(-1);
    }
    /* COMET II仮想マシンのリセット */
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
