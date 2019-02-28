#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "word.h"
#include "struct.h"
#include "cerr.h"

/**
 * @brief ファイル読み込みのエラー定義
 */
static CERR cerr_load[] = {
    { 210, "load - memory overflow" },
    { 211, "object file not specified" },
    { 212, "invalid option" },
    { 213, "invalid argument" },
};

/* load.hで定義された関数群 */
void addcerrlist_load()
{
    addcerrlist(ARRAYSIZE(cerr_load), cerr_load);
}

WORD loadassemble(const char *file, WORD start)
{
    FILE *fp = NULL;
    WORD end = 0;

    assert(file != NULL);
    if((fp = fopen(file, "rb")) == NULL) {
        perror(file);
        return 0;
    }
    end = start + fread(sys->memory + start, sizeof(WORD), sys->memsize - start, fp);
    if(end == sys->memsize) {
        setcerr(210, file);    /* load - memory overflow */
        fprintf(stderr, "Load error - %d: %s\n", cerr->num, cerr->msg);
    }
    fclose(fp);
    return end;
}
