#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <getopt.h>
#include <assert.h>

#include "exec.h"
#include "cmem.h"
#include "cerr.h"
#include "package.h"

char *grstr(WORD word)
{
    char *str;
    str = (char *)malloc(4);
    sprintf(str, "GR%d", word);
    return str;
}

bool disassemble(const char *file)
{
    FILE *fp;
    bool stat = true;
    int i = 0;
    WORD w, cmd, r, x, r1, r2, adr;
    CMDTYPE type = 0;
    char *cmdname;

    assert(file != NULL);
    if((fp = fopen(file, "rb")) == NULL) {
        perror(file);
        return false;
    }

    create_code_cmdtype();                          /* 命令のコードとタイプがキーのハッシュ表を作成 */

    fprintf(stdout, "MAIN\tSTART\n");
    for( ; !feof(fp); ) {
        fread(&w, sizeof(WORD), 1, fp);
        cmd = w & 0xFF00;
        cmdname = getcmdname(cmd);
        if(cmd == 0xFF00 || (w != 0 && cmd == 0x0000)) {
            fprintf(stdout, "\tDC\t%d\t\t; #%04X", w, i++);
        } else if((type = getcmdtype(cmd)) == R_ADR_X || type == ADR_X) {
            fread(&adr, sizeof(WORD), 1, fp);
            fprintf(stdout, "\t%s\t", cmdname);
            if(type == R_ADR_X) {
                r = (w &  0x00F0) >> 4;
                fprintf(stdout, "%s,", grstr(r));
            }
            fprintf(stdout, "#%04X", adr);
            if((x = w &  0x000F) != 0) {
                fprintf(stdout, ",%s", grstr(x));
            }
            fprintf(stdout, "\t\t; #%04X", i);
            i += 2;
        } else {
            fprintf(stdout, "\t%s", cmdname);
            if(type == R1_R2) {
                r1 = (w &  0x00F0) >> 4;
                r2 = w &  0x000F;
                fprintf(stdout, "\t%s,%s", grstr(r1), grstr(r2));
            } else if(type == R_) {
                r = (w & 0x00F0) >> 4;
                fprintf(stdout, "\t%s", grstr(r));
            }
            fprintf(stdout, "\t\t; #%04X", i++);
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "\tEND\n");
    free_code_cmdtype();
    return stat;
}

/**
 * disassembleコマンドのオプション
 */
static struct option longopts[] = {
    {"version", no_argument, NULL, 'v' },
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

/**
 * @brief disassembleコマンドのメイン
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
    disassemble(argv[optind]);                /* プログラム実行 */
    stat = (cerr->num == 0) ? 0 : 1;
    /* エラーの解放 */
    freecerr();
    return stat;
}
