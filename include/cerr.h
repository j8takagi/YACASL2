#ifndef YACASL2_CERR_H_INCLUDED
#define YACASL2_CERR_H_INCLUDED

#include <stdbool.h>
#include "cmem.h"

/**
 * エラーの構造体
 */
typedef struct _CERR {
    int num;        /**<エラー番号 */
    char *msg;      /**<エラーメッセージ */
} CERR;

/**
 * 現在のエラー
 */
extern CERR *cerr;

/**
 * エラーリスト型
 */
typedef struct _CERRLIST {
    struct _CERRLIST *next;     /**<リスト次項目へのポインタ */
    CERR *cerr;                 /**<エラーの構造体 */
} CERRLIST;

/**
 * エラーリスト
 */
extern CERRLIST *cerrlist;

enum {
    CERRSTRSIZE = 10,    /**<エラーメッセージ中に挿入できる文字列のサイズ */
    CERRMSGSIZE = 70,    /**<エラーメッセージのサイズ */
};

/**
 * エラーの初期化
 */
void cerr_init();

/**
 * エラーリストを作成・追加する
 */
void addcerrlist(int cerrc, CERR cerrv[]);

/**
 * エラーリストを表示する
 */
void printcerrlist();

/**
 * 現在のエラーを設定する
 */
void setcerr(int num, const char *str);

/**
 * エラーリストから、エラー番号に対応するメッセージを返す
 */
char *getcerrmsg(int num);

/**
 * エラーリストと現在のエラーを解放する
 */
void freecerr();
#endif
