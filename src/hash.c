#include "hash.h"

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
