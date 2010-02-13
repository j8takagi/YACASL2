#ifndef YACASL2_CERR_H_INCLUDED
#define YACASL2_CERR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef ARRAYSIZE
#define ARRAYSIZE(array) (sizeof(array)/sizeof(array[0]))
#endif

/* エラー番号 */
extern int cerrno;

/* エラーメッセージ */
extern char *cerrmsg;

/* エラーコードリスト */
typedef struct {
    int num;
    char *msg;
} CERRARRAY;

/* エラーメッセージ */
extern CERRARRAY cerr[];

enum {
    CERRSTRSIZE = 10,    /* エラーメッセージ中に挿入できる文字列のサイズ */
    CERRMSGSIZE = 70,    /* エラーメッセージのサイズ */
};

/* エラー番号とエラーメッセージを設定 */
void setcerr(int num, const char *str);

/* エラー番号からメッセージを返す */
char *getcerrmsg(int num);

/* エラーを解放する */
void freecerr();
#endif
