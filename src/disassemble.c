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
    char *str = malloc_chk(3 + 1, "grstr.str");
    sprintf(str, "GR%d", word);
    return str;
}

bool disassemble(const char *file)
{
    bool stat = true;
    FILE *fp;
    WORD i = 0, w, cmd, r, x, r1, r2, adr;
    CMDTYPE cmdtype = 0;
    char *cmdname, *g1, *g2;

    assert(file != NULL);
    if((fp = fopen(file, "rb")) == NULL) {
        perror(file);
        return false;
    }

    create_code_cmdtype();                          /* 命令のコードとタイプがキーのハッシュ表を作成 */

    fprintf(stdout, "MAIN\tSTART\n");
    for(; ;) {
        fread(&w, sizeof(WORD), 1, fp);
        if(feof(fp)) {
            break;
        }
        cmd = w & 0xFF00;
        cmdname = getcmdname(cmd);
        cmdtype = getcmdtype(cmd);
        if(cmd == 0xFF00 || (w != 0 && cmd == 0x0000)) {
            fprintf(stdout, "\tDC\t%d\t\t\t\t; #%04X: #%04X :: ", w, i++, w);
            print_dumpword(w, true);
        } else if(cmdtype == R_ADR_X || cmdtype == ADR_X) {
            fread(&adr, sizeof(WORD), 1, fp);
            fprintf(stdout, "\t%s\t", cmdname);
            if(cmdtype == R_ADR_X) {
                r = (w & 0x00F0) >> 4;
                fprintf(stdout, "%s,", (g1 = grstr(r)));
                FREE(g1);
            }
            fprintf(stdout, "#%04X", adr);
            if((x = w & 0x000F) != 0) {
                fprintf(stdout, ",%s", (g1 = grstr(x)));
                FREE(g1);
            }
            fprintf(stdout, "\t\t\t\t; #%04X: #%04X #%04X", i, w, adr);
            i += 2;
        } else {
            fprintf(stdout, "\t%s", cmdname);
            if(cmdtype == R1_R2) {
                r1 = (w & 0x00F0) >> 4;
                r2 = w & 0x000F;
                fprintf(stdout, "\t%s,%s", (g1=grstr(r1)), (g2=grstr(r2)));
                FREE(g1);
                FREE(g2);
            } else if(cmdtype == R_) {
                r = (w & 0x00F0) >> 4;
                fprintf(stdout, "\t%s", (g1 = grstr(r)));
                FREE(g1);
            }
            fprintf(stdout, "\t\t\t\t; #%04X: #%04X", i++, w);
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "\tEND\n");
    free_code_cmdtype();
    fclose(fp);
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
