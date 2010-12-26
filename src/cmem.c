#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "cmem.h"

/**
 * mallocを実行し、0で初期化
 * メモリを確保できない場合はエラーを出力して終了
 */
void *malloc_chk(size_t size, char *tag)
{
    void *p;

    if((p = malloc(size)) == NULL) {
        fprintf(stderr, "%s: cannot allocate memory\n", tag);
        exit(-1);
    }
    return memset(p, 0, size);
}

/**
 * callocを実行
 * メモリを確保できない場合はエラーを出力して終了
 */
void *calloc_chk(size_t nmemb, size_t size, char *tag)
{
    void *p;

    if((p = calloc(nmemb, size)) == NULL) {
        fprintf(stderr, "%s: cannot allocate memory\n", tag);
        exit(-1);
    }
    return p;
}

/**
 * malloc_chkを実行してメモリを確保してから、コピーした文字列を返す
 */
char *strdup_chk(const char *s, char *tag)
{
    assert(s != NULL);
    char *t;

    t = malloc_chk(strlen(s) + 1, tag);
    strcpy(t, s);
    return t;
}

/**
 * メモリを解放
 */
void free_chk(void *ptr, char *tag)
{
    free(ptr);
}
