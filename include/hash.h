#ifndef YACASL2_HASH_INCLUDED
#define YACASL2_HASH_INCLUDED

#ifndef ARRAYSIZE
#define ARRAYSIZE(array) (sizeof(array)/sizeof(array[0]))
#endif

/* ハッシュ共用体の型 */
typedef enum {
    CHARS = 0,
    INT = 1,
} UTYPE;

/* ハッシュ共用体 */
typedef struct {
    UTYPE type;
    union {
        char *s;
        int i;
    } val;
} HKEY;

/* ハッシュ表のサイズを決めるため、引数の数値未満で最大の素数を返す */
int hashtabsize(int size);

/* ハッシュ値を取得する */
unsigned hash(int keyc, HKEY *keyv[], int tabsize);

#endif
