#include <stdio.h>
#include "cmem.h"

int main(){
    char *dst, *src = "abcdefghijklmnopqrstuvwxyz";

    dst = strdup_chk(src, "strdup_chk unit test");
    printf("%s\n", dst);
    FREE(dst);
    return 0;
}
