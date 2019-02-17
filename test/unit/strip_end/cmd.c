#include <stdio.h>
#include "cmem.h"

int main(){
    char s[][10] = {"abc\n", "abc    ", "abc\t", "abc\t\n", "abc"};

    for(size_t i = 0; i < sizeof(s) / sizeof(s[0]); i++) {
        printf("%s:\n", strip_end(s[i]));
    }
    return 0;
}
