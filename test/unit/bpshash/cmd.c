#include "debugger.h"

int main()
{
    int i;
    WORD w[10] ={0, 1, 2, 3, 10, 250, 251, 254, 0x8FFF, 65535};
    for(i = 0; i < 10; i++) {
        printf("%ld: %d\n", (long)w[i], adrhash(w[i]));
    }
    return 0;
}
