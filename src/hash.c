#include "hash.h"

/* ハッシュ表のサイズを決めるため、引数の数値より大きい最小の素数を返す */
int hashtabsize(int size)
{
    int i;
    const int prime[] =
        {2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
         31, 37, 41, 43, 47, 53, 59, 61, 67,
         71, 73, 79, 83, 89, 97,
        };
    for(i = 0; i < ARRAYSIZE(prime); i++) {
        if(i > 0 && prime[i] >= size) {
            break;
        }
    }
    return prime[i];
}

/* ハッシュ値を取得する */
unsigned hash(int keyc, HKEY *keyv[], int tabsize)
{
    int i;
    char *p;
    unsigned hashval = 0;

    for(i = 0; i < keyc; i++) {
        switch(keyv[i]->type) {
        case CHARS:
            for(p = keyv[i]->val.s; *p != '\0'; p++) {
                hashval = *p + 31 * hashval;
            }
            break;
        case INT:
            hashval = keyv[i]->val.i + 31 * hashval;
            break;
        default:
            break;
        }
    }
    return (hashval % tabsize);
}
