#include "cerr.h"

/* エラーメッセージ */
int cerrno = 0;
char *cerrmsg;

/* エラー番号とエラーメッセージを設定する */
void setcerr(int num, const char *val)
{
    assert(cerr != NULL && num > 0);

    cerrno = num;
    cerrmsg = malloc(MSGSIZE + 1);
    if(val != NULL) {
        strcpy(cerrmsg, val);
        strcat(cerrmsg, ": ");
        strcat(cerrmsg, getcerrmsg(cerrno));
    } else {
        strcpy(cerrmsg, getcerrmsg(cerrno));
    }
}

/* エラー番号からメッセージを返す */
char *getcerrmsg(int num)
{
    assert(cerr != NULL && num > 0);
    int i = 0;
    CERRARRAY *ptr;

    do {
        if((ptr = &cerr[i++])->num == num) {
            return ptr->msg;
        }
    } while(ptr->num > 0);
    return "unkown error";
}

/* エラーを解放する */
void freecerr()
{
    assert(cerrno > 0);
    cerrno = 0;
    if(strlen(cerrmsg) > 0) {
        free(cerrmsg);
    }
}
