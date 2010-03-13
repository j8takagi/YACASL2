#include "cerr.h"

/* エラー番号 */
int cerrno = 0;

/* エラーメッセージ */
char *cerrmsg;

/* エラーリスト */
CERRLIST *cerr;

/* エラーリストを作成する */
bool addcerrlist(int newerrc, CERRARRAY newerrv[])
{
    int i;
    CERRLIST *p, *q;

    assert(newerrc > 0 && newerrv != NULL);
    if(cerr != NULL) {
        for(p = cerr; p != NULL; p = p->next) {
            q = p;
        }
        if((p = q->next = malloc(sizeof(CERRLIST))) == NULL) {
            goto addcerrlisterr;
        }
    } else if((p = cerr = malloc(sizeof(CERRLIST))) == NULL) {
        goto addcerrlisterr;
    }
    for(i = 0; i < newerrc; i++) {
        p->err = &(newerrv[i]);
        if((p->next = malloc(sizeof(CERRLIST))) == NULL) {
            goto addcerrlisterr;
        }
        q = p;
        p = p->next;
    }
    q->next = NULL;
    return true;
addcerrlisterr:
    fprintf(stderr, "addcerrlist: cannot allocate memory\n");
    exit(-1);
}

/* エラー番号とエラーメッセージを設定する */
void setcerr(int num, const char *str)
{
    cerrno = num;
    cerrmsg = malloc(CERRMSGSIZE + 1);
    if(str != NULL && strlen(str) <= CERRSTRSIZE) {
        sprintf(cerrmsg, "%s: %s", str, getcerrmsg(cerrno));
    } else {
        strcpy(cerrmsg, getcerrmsg(cerrno));
    }
}

/* リストから、エラー番号に対応するメッセージを返す */
char *getcerrmsg(int num)
{
    CERRLIST *p;

    for(p = cerr; p != NULL; p = p->next) {
        if(num == p->err->num) {
            return p->err->msg;
        }
    }
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
