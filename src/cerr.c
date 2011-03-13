#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "cerr.h"

/**
 * エラーの初期化
 */
void cerr_init()
{
    cerr = malloc_chk(sizeof(CERR), "cerr");
    cerr->num = 0;
}

/**
 * 現在のエラー
 */
CERR *cerr;

/**
 * エラーリスト
 */
CERRLIST *cerrlist;

/**
 * エラーリストを作成または追加する
 */
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

/**
 * エラーリストを表示する
 */
void printcerrlist()
{
    CERRLIST *p;

    if(cerrlist == NULL) {
        puts("error list is null.");
    } else {
        for(p = cerrlist; p != NULL; p = p->next) {
            printf("%d: %s\n", p->cerr->num, p->cerr->msg);
        }
    }
}

/**
 * 現在のエラーを設定する
 */
void setcerr(int num, const char *str)
{
    /* 現在のエラー番号を設定  */
    cerr->num = num;
    /* 現在のエラーメッセージを設定 */
    cerr->msg = malloc_chk(CERRMSGSIZE + 1, "cerr.msg");
    if(str != NULL && strlen(str) <= CERRSTRSIZE) {
        sprintf(cerr->msg, "%s: %s", str, getcerrmsg(cerr->num));
    } else {
        strcpy(cerr->msg, getcerrmsg(cerr->num));
    }
}

/**
 * エラーリストから、エラー番号に対応するメッセージを返す
 */
char *getcerrmsg(int num)
{
    CERRLIST *p;

    for(p = cerrlist; p != NULL; p = p->next) {
        if(num == p->cerr->num) {
            return p->cerr->msg;
        }
    }
    return "unknown error";
}

/**
 * エラーリストと現在のエラーを解放する
 */
void freecerr()
{
    CERRLIST *p = cerrlist, *q;

    /* 現在のエラーメッセージを解放 */
    free_chk(cerr->msg, "cerr.msg");
    /* 現在のエラーを解放 */
    free_chk(cerr, "cerr");
    /* エラーリストを解放 */
    for(p = cerrlist; p != NULL; p = q) {
        q = p->next;
        /* free_chk(p->cerr->msg, "freecerr.p.cerr.msg"); */
        /* free_chk(p->cerr, "freecerr.p.cerr"); */
        free_chk(p, "freecerr.p");
    }
}
