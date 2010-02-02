#include <stdio.h>
#include "hash.h"
#include <string.h>
#include <stdlib.h>

unsigned hash_char2(int tsize)
{
    char *str[2] = {"abc", "123"};
    int i;
    HKEY *keys[2];

    /* ハッシュ共用体の設定 */
    for(i = 0; i < 2; i++) {
        keys[i] = malloc(sizeof(HKEY));
        keys[i]->type = CHARS;
        keys[i]->val.s = strdup(str[i]);
    }
    /* ハッシュ値を取得する */
    return hash(2, keys, tsize);
}

unsigned hash_int2(int tsize)
{
    int num[2] = {19, 11}, i;
    HKEY *keys[2];

    /* ハッシュ共用体の設定 */
    for(i = 0; i < 2; i++) {
        keys[i] = malloc(sizeof(HKEY));
        keys[i]->type = INT;
        keys[i]->val.i = num[i];
    }
    /* ハッシュ値を取得する */
    return hash(2, keys, tsize);
}

unsigned hash_char2_int2(int tsize)
{
    char *str[2] = {"abc", "123"};
    int num[2] = {19, 11}, i;
    HKEY *keys[4];

    /* ハッシュ共用体の設定 */
    for(i = 0; i < 2; i++) {
        keys[i] = malloc(sizeof(HKEY));
        keys[i]->type = CHARS;
        keys[i]->val.s = strdup(str[i]);
    }
    for(i = 2; i < 4; i++) {
        keys[i] = malloc(sizeof(HKEY));
        keys[i]->type = INT;
        keys[i]->val.i = num[i];
    }
    /* ハッシュ値を取得する */
    return hash(4, keys, tsize);
}

int main()
{
    int tsize = 31;

    printf("HASH VALUE: %d\n", hash_char2(tsize));
    printf("HASH VALUE: %d\n", hash_int2(tsize));
    printf("HASH VALUE: %d\n", hash_char2_int2(tsize));
    return 0;
}
