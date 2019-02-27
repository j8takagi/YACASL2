#include "hash.h"

unsigned hash(int keyc, HKEY *keyv[], int tabsize)
{
    unsigned hashval = 0;
    enum {
        HASHNUM = 31
    };

    for(int i = 0; i < keyc; i++) {
        switch(keyv[i]->type) {
        case CHARS:
            for(int j = 0; keyv[i]->val.s[j]; j++) {
                hashval = keyv[i]->val.s[j] + HASHNUM * hashval;
            }
            break;
        case INT:
            hashval = keyv[i]->val.i + HASHNUM * hashval;
            break;
        default:
            break;
        }
    }
    return (hashval % tabsize);
}
