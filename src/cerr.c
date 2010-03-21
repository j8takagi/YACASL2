#include "cerr.h"

/* mallocを実行し、メモリを確保できない場合は */
/* エラーを出力して終了 */
void *malloc_chk(size_t size, char *tag)
{
    void *p;

    if((p = malloc(size)) == NULL) {
        fprintf(stderr, "%s: cannot allocate memory\n", tag);
        exit(-1);
    }
    return p;
}

/* 現在のエラー */
CERR *cerr;

/* エラーリスト */
CERRLIST *cerrlist;

/* エラーリストを作成・追加する */
bool addcerrlist(int newerrc, CERR newerrv[])
{
    int i;
    CERRLIST *p, *q;

    assert(newerrc > 0 && newerrv != NULL);
    if(cerrlist == NULL) {
        p = cerrlist = malloc_chk(sizeof(CERRLIST), "cerrlist");
    } else {
        for(p = cerrlist; p != NULL; p = p->next) {
            q = p;
        }
        p = q->next = malloc_chk(sizeof(CERRLIST), "cerrlist.next");
    }
    for(i = 0; i < newerrc; i++) {
        p->cerr = &(newerrv[i]);
        p->next = malloc_chk(sizeof(CERRLIST), "cerrlist.next");
        q = p;
        p = p->next;
    }
    q->next = NULL;
    return true;
}

/* 現在のエラーを設定する */
void setcerr(int num, const char *str)
{
    cerr->num = num;
    cerr->msg = malloc_chk(CERRMSGSIZE + 1, "cerr.msg");
    if(str != NULL && strlen(str) <= CERRSTRSIZE) {
        sprintf(cerr->msg, "%s: %s", str, getcerrmsg(cerr->num));
    } else {
        strcpy(cerr->msg, getcerrmsg(cerr->num));
    }
}

/* エラーリストから、エラー番号に対応するメッセージを返す */
char *getcerrmsg(int num)
{
    CERRLIST *p;

    for(p = cerrlist; p != NULL; p = p->next) {
        if(num == p->cerr->num) {
            return p->cerr->msg;
        }
    }
    return "unkown error";
}

/* エラーリストと現在のエラーを解放する */
void freecerr()
{
    CERRLIST *p = cerrlist, *q;

    /* エラーリストを解放 */
    while(p != NULL) {
        q = p->next;
        free(p);
        p = q;
    }
    /* 現在のエラーを解放 */
    free(cerr);
}
