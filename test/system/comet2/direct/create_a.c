#include <stdio.h>
#include <stdlib.h>
#include "cmem.h"
#include "word.h"

/**
 * 引数で指定したファイルにアセンブル結果を書込
 */
int main()
{
    FILE *fp;
    const WORD w[] = {0x1010, 0x0005, 0x2210, 0x0006, 0x8100, 0x0003, 0x0001};
    const char *f = "a.comet";

    if((fp = fopen(f, "w")) == NULL) {
        perror(f);
        exit(-1);
    }
    fwrite(w, sizeof(WORD), ARRAYSIZE(w), fp);
    fclose(fp);
    return 0;
}