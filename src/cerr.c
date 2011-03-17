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
    cerr = malloc_chk(sizeof(CERR *), "cerr");
    cerr->num = 0;
}

/**
 * 現在のエラー
 */
CERR *cerr;

/**
 * エラーリスト
 */
CERRLIST *cerrlist = NULL;

/**
 * エラーリストを作成または追加する
 */
void addcerrlist(int errc, CERR errv[])
{
    int i;
    CERRLIST *p = NULL, *q = malloc_chk(sizeof(CERRLIST *), "cerrlist");

    assert(errc > 0 && errv != NULL);
    for(i = 0; i < errc; i++) {
        if(p == NULL) {
            p = q;
        } else {
            p = p->next = malloc_chk(sizeof(CERRLIST *), "cerrlist.next");
        }
        p->cerr = &errv[i];
        p->next = NULL;
    }
    p->next = cerrlist;
    cerrlist = q;
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
    char *msg = "unknown error";

    for(p = cerrlist; p != NULL; p = p->next) {
        if(num == p->cerr->num) {
            msg = p->cerr->msg;
            break;
        }
    }
    return msg;
}

/**
 * エラーリストと現在のエラーを解放する
 */
void freecerr()
{
    CERRLIST *p = cerrlist, *q;

    /* 現在のエラーメッセージを解放 */
    FREE(cerr->msg);
    /* 現在のエラーを解放 */
    FREE(cerr);
    /* エラーリストを解放 */
    for(p = cerrlist; p != NULL; p = q) {
        q = p->next;
        FREE(p);
    }
}
