#ifndef YACASL2_HASH_INCLUDED
#define YACASL2_HASH_INCLUDED

/**
 * ハッシュ共用体の型
 */
typedef enum {
    CHARS = 0,
    INT = 1,
} UTYPE;

/**
 * ハッシュ共用体
 */
typedef struct {
    UTYPE type;                 /**<ハッシュ値の元データのデータ型 */
    union {
        char *s;                /**<char型のデータ */
        int i;                  /**<int型のデータ */
    } val;                      /**<ハッシュ値の元データ */
} HKEY;

/**
 * ハッシュ値を取得する
 */
unsigned hash(int keyc, HKEY *keyv[], int tabsize);

#endif
