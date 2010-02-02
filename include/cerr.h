#ifndef YACASL2_CERR_H_INCLUDED
#define YACASL2_CERR_H_INCLUDED

#include <malloc.h>
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

/* エラー番号とエラーメッセージを設定 */
void setcerr(int num, const char *val);

/* エラー番号からメッセージを返す */
char *getcerrmsg(int num);

/* エラーを解放する */
void freecerr();
#endif
