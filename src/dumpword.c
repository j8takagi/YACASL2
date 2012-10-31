#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "word.h"
#include "cerr.h"
#include "package.h"

/**
 * dumpwordコマンドのオプション
 */
static struct option longopts[] = {
    { "arithmetic", no_argument, NULL, 'a' },
    { "logical", no_argument, NULL, 'l' },
    { "version", no_argument, NULL, 'v' },
    { "help", no_argument, NULL, 'h' },
    { 0, 0, 0, 0 },
};

/**
 * dumpwordコマンドのメイン
 */
int main(int argc, char *argv[])
{
    bool logicalmode = false;    /* レジストリの内容を論理値（0から65535）で表示する場合はtrue */
    int opt;
    WORD word;
    const char *version = PACKAGE_VERSION,  *cmdversion = "dumpword of YACASL2 version %s\n";
    const char *usage = "Usage: %s [-alh] WORD\n";

    cerr_init();
    addcerrlist_word();
    while((opt = getopt_long(argc, argv, "alvh", longopts, NULL)) != -1) {
        switch(opt) {
        case 'l':
            logicalmode = true;
            break;
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
        fprintf(stderr, usage, argv[0]);
        exit(1);
    }
    /* WORD値に変換 */
    word = nh2word(argv[optind]);
    if(cerr->num > 0) {
        fprintf(stderr, "Dumpword Error - %d: %s\n", cerr->num, cerr->msg);
        exit(1);
    }
    fprintf(stdout, "%6s: ", argv[optind]);
    print_dumpword(word, logicalmode);
    return 0;
}
