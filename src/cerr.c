#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "cerr.h"

void cerr_init()
{
    cerr = malloc_chk(sizeof(CERR), "cerr");
    cerr->num = 0;
}

CERR *cerr;

CERRLIST *cerrlist = NULL;

void addcerrlist(int cerrc, CERR cerrv[])
{
    int i;
    CERRLIST *p = NULL, *q = malloc_chk(sizeof(CERRLIST), "cerrlist");

    assert(cerrc > 0 && cerrv != NULL);
    for(i = 0; i < cerrc; i++) {
        if(p == NULL) {
            p = q;
        } else {
            p = p->next = malloc_chk(sizeof(CERRLIST), "cerrlist.next");
        }
        p->cerr = &cerrv[i];
        p->next = NULL;
    }
    p->next = cerrlist;
    cerrlist = q;
}

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

void setcerr(int num, const char *str)
{
    /* 現在のエラー番号を設定  */
    cerr->num = num;
    /* 現在のエラーメッセージを設定 */
    cerr->msg = malloc_chk(CERRMSGSIZE + 1, "cerr.msg");
    if(0 < strlen(str) && strlen(str) <= CERRSTRSIZE) {
        sprintf(cerr->msg, "%s: %s", str, getcerrmsg(cerr->num));
    } else {
        strcpy(cerr->msg, getcerrmsg(cerr->num));
    }
}

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
