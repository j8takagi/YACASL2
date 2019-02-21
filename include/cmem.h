#ifndef YACASL2_CMEM_H_INCLUDED
#define YACASL2_CMEM_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

/**
 * @brief 配列のサイズを返すマクロ
 */
#ifndef ARRAYSIZE
#define ARRAYSIZE(array) (sizeof(array)/sizeof(array[0]))
#endif

/**
 * @brief メモリを解放するマクロ
 */
#ifndef FREE
#define FREE(ptr) {free(ptr); ptr = NULL;}
#endif

/**
 * @brief mallocを実行し、0で初期化する
 *
 * メモリを確保できない場合はエラーを出力して終了
 *
 * @return なし
 *
 * @param size メモリーのサイズ
 * @param tag エラーメッセージなどで表示されるタグ
 */
void *malloc_chk(size_t size, const char *tag);

/**
 * @brief 領域の数とサイズを指定してメモリーを確保するcallocを実行する
 *
 * メモリを確保できない場合はエラーを出力して終了
 *
 * @return なし
 *
 * @param nmemb 領域の数
 * @param size 領域1個あたりのメモリーサイズ
 * @param tag エラーメッセージなどで表示されるタグ
 */
void *calloc_chk(size_t nmemb, size_t size, const char *tag);

/**
 * @brief malloc_chkを実行してメモリを確保し、コピーした文字列を返す
 *
 * @return コピーした文字列
 *
 * @param s 文字列
 * @param tag エラーメッセージなどで表示されるタグ
 */
char *strdup_chk(const char *s, const char *tag);

/**
 * @brief malloc_chkを実行してメモリを確保し、コピーした文字列の指定した長さの部分を返す
 *
 * @return コピーした文字列
 *
 * @param s 文字列
 * @param len 文字列の長さ
 * @param tag エラーメッセージなどで表示されるタグ
 */
char *strndup_chk(const char *s, size_t len, const char *tag);

/**
 * @brief 文字列の末尾から、改行と空白とタブを削除する
 *
 * @return 末尾から改行と空白とタブを削除した文字列
 *
 * @param s 文字列
 */
char *strip_end(char *s);

/**
 * @brief 文字列から「'」以降の文字列をCASL IIのコメントとして削除する。「''」の場合は除く
 *
 * @return コメントを削除した文字列
 *
 * @param s 文字列
 */
char *strip_casl2_comment(char *s);

#endif
