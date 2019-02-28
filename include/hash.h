#ifndef YACASL2_HASH_H_INCLUDED
#define YACASL2_HASH_H_INCLUDED

/**
 * @brief ハッシュ共用体の数値
 */
typedef enum {
    CHARS = 0,                  /**<char型のデータ */
    INT = 1,                    /**<int型のデータ  */
} UTYPE;

/**
 * @brief ハッシュ共用体のデータ型
 */
typedef struct {
    UTYPE type;                 /**<ハッシュ値の元データのデータ型 */
    union {
        char *s;                /**<char型のデータ */
        int i;                  /**<int型のデータ */
    } val;                      /**<ハッシュ値の元データ */
} HKEY;

/**
 * @brief ハッシュ値を取得する
 *
 * @return ハッシュ値
 *
 * @param keyc キーの数
 * @param *keyv[] キーを表す配列
 * @param tabsize テーブルのサイズ
 */
unsigned hash(int keyc, HKEY *keyv[], int tabsize);

#endif
