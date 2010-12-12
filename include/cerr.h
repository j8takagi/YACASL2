#ifndef YACASL2_CERR_H_INCLUDED
#define YACASL2_CERR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "cmem.h"

/* エラーの構造体 */
typedef struct {
    int num;        /* エラー番号 */
    char *msg;      /* エラーメッセージ */
} CERR;

/* 現在のエラー */
extern CERR *cerr;

/* エラーリスト */
typedef struct _CERRLIST {
    struct _CERRLIST *next;
    CERR *cerr;
} CERRLIST;

extern CERRLIST *cerrlist;

enum {
    CERRSTRSIZE = 10,    /* エラーメッセージ中に挿入できる文字列のサイズ */
    CERRMSGSIZE = 70,    /* エラーメッセージのサイズ */
};

/* エラーの初期化 */
void cerr_init();

/* エラーリストを作成・追加する */
bool addcerrlist(int cerrc, CERR cerrv[]);

/* エラー番号とエラーメッセージを設定 */
void setcerr(int num, const char *str);

/* エラー番号からメッセージを返す */
char *getcerrmsg(int num);

/* エラーを解放する */
void freecerr();
#endif
