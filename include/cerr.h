#ifndef YACASL2_CERR_H_INCLUDED
#define YACASL2_CERR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "cmem.h"

/**
 * @brief エラーを表すデータ型
 */
typedef struct _CERR {
    int num;        /**<エラー番号 */
    char *msg;      /**<エラーメッセージ */
} CERR;

/**
 * @brief 現在のエラー
 */
extern CERR *cerr;

/**
 * @brief エラーリストのデータ型
 */
typedef struct _CERRLIST {
    struct _CERRLIST *next;     /**<リスト次項目へのポインタ */
    CERR *cerr;                 /**<エラーの構造体 */
} CERRLIST;

/**
 * @brief エラーリスト
 */
extern CERRLIST *cerrlist;

enum {
    CERRSTRSIZE = 10,    /**<エラーメッセージ中に挿入できる文字列のサイズ */
    CERRMSGSIZE = 70,    /**<エラーメッセージのサイズ */
};

/**
 * @brief エラーを初期化する
 *
 */
void cerr_init();

/**
 * @brief エラーリストを作成・追加する
 *
 *
 * @param cerrc 作成または追加するエラーの数
 * @param cerrv 作成または追加するエラーの配列
 */
void addcerrlist(int cerrc, CERR cerrv[]);

/**
 * @brief エラーリストを表示する
 *
 */
void printcerrlist();

/**
 * @brief 現在のエラーを設定する
 *
 * @param num エラー番号
 * @param *str エラーメッセージに含まれる文字列
 */
void setcerr(int num, const char *str);

/**
 * @brief エラー番号に対応するエラーメッセージを返す
 *
 * @return エラーメッセージ
 *
 * @param num エラー番号
 */
const char *getcerrmsg(int num);

/**
 * @brief エラーリストと現在のエラーを解放する
 *
 */
void freecerr();

/**
 * @brief 引数が無視されることを表示する
 *
 * @param argc 引数の配列数
 * @param *argv 引数配列
 */
void warn_ignore_arg(int argc, char *argv[]);
#endif
