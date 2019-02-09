#include <stdio.h>
#include "cmem.h"

int main(){
    char *dst, *src = "abcdefghijklmnopqrstuvwxyz";
    size_t len = 10;

    dst = strndup_chk(src, len, "strndup_chk unit test");
    printf("%s\n", dst);
    FREE(dst);
    return 0;
}
