#include "casl2.h"
#define _GNU_SOURCE
#include <getopt.h>

static struct option longopts[] = {
    {"arithmetic", no_argument, NULL, 'a'},
    {"logical", no_argument, NULL, 'l'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0}
};

int main(int argc, char *argv[])
{
    int opt;
    WORD w;
    char *check;
    const char *usage = "Usage: %s [-alh] WORD\n";

    logicalmode = false;
    while((opt = getopt_long(argc, argv, "alh", longopts, NULL)) != -1) {
        switch(opt) {
        case 'l':
            logicalmode = true;
            break;
        case 'h':
            fprintf(stdout, usage, argv[0]);
            exit(-1);
        case '?':
            fprintf(stderr, usage, argv[0]);
            exit(-1);
        }
    }

    if(argv[optind] == NULL) {
        fprintf(stderr, usage, argv[0]);
        exit(-1);
    }
    if(*argv[optind] == '-' || strlen(argv[optind]) > 4) {
        setcerr(116, argv[optind]);    /* out of hex range */
    }
    /* WORD値に変換 */
    w = (WORD)strtol(argv[optind], &check, 16);
    if(*check != '\0') {
        setcerr(115, argv[optind]);    /* not hex */
    }
    if(cerrno > 0) {
        fprintf(stderr, "Dumpword Error - %d: %s\n", cerrno, cerrmsg);
        exit(-1);
    }
    if(logicalmode == true) {
        fprintf(stdout, "%4s: %6d = #%04X = %s\n", argv[optind], w, w, word2bit(w));
    } else {
        fprintf(stdout, "%4s: %6d = #%04X = %s\n", argv[optind], (short)w, w, word2bit(w));
    }
    return 0;
}
