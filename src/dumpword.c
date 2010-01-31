#include "casl2.h"
#define _GNU_SOURCE
#include <getopt.h>

static struct option longopts[] = {
    {"arithmetic", no_argument, NULL, 'a'},
    {"logical", no_argument, NULL, 'l'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

int main(int argc, char *argv[])
{
    int opt;
    WORD word;
    const char *usage = "Usage: %s [-alh] WORD\n";

    logicalmode = false;
    while((opt = getopt_long(argc, argv, "alh", longopts, NULL)) != -1) {
        switch(opt) {
        case 'l':
            logicalmode = true;
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
        fprintf(stderr, usage, argv[0]);
        exit(-1);
    }
    /* WORD値に変換 */
    word = a2word(argv[optind]);
    if(cerrno > 0) {
        fprintf(stderr, "Dumpword Error - %d: %s\n", cerrno, cerrmsg);
        exit(-1);
    }
    if(logicalmode == true) {
        fprintf(stdout, "%6s: %6d = #%04X = %s\n", argv[optind], word, word, word2bit(word));
    } else {
        fprintf(stdout, "%6s: %6d = #%04X = %s\n", argv[optind], (short)word, word, word2bit(word));
    }
    return 0;
}
