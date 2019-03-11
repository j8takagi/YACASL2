#include <stdio.h>
#include "cmem.h"

int main()
{
    char *src[] = {"abcdefghij", "abcde"};
    char *dst = NULL;

    for(unsigned i = 0; i < ARRAYSIZE(src); i++) {
        dst = strrev(src[i]);
        puts(dst);
        FREE(dst);
    }
    return 0;
}
