#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cerr.h"
#include "cmem.h"
#include "hash.h"
#include "assemble.h"

static int labelcnt = 0;                /* ラベル数 */
static LABELTAB *labels[LABELTABSIZE];  /* ラベル表 */

/**
 * プログラム名とラベルに対応するハッシュ値を返す
 */
unsigned labelhash(const char *prog, const char *label)
{
    HKEY *keys[2];
    int i = 0, j;
    unsigned h;

    if(prog != NULL) {
        keys[i] = malloc_chk(sizeof(HKEY), "labelhash.key");
        keys[i]->type = CHARS;
        keys[i]->val.s = strdup_chk(prog, "labelhash.key.val");
        i++;
    }
    keys[i] = malloc_chk(sizeof(HKEY), "labelhash.key");
    keys[i]->type = CHARS;
    keys[i]->val.s = strdup_chk(label, "labelhash.key.val");
    h = hash(i+1, keys, LABELTABSIZE);
    for(j = 0; j < i + 1; j++) {
        FREE(keys[j]->val.s);
        FREE(keys[j]);
    }
    return h;
}

/**
 * プログラム名とラベルに対応するアドレスをラベル表から検索する
 */
WORD getlabel(const char *prog, const char *label)
{
    assert(label != NULL);
    LABELTAB *p;

    for(p = labels[labelhash(prog, label)]; p != NULL; p = p->next) {
        if((prog == NULL || (p->prog != NULL && strcmp(prog, p->prog) == 0)) &&
           strcmp(label, p->label) == 0)
        {
            return p->adr;
        }
    }
    return 0xFFFF;
}

/**
 * プログラム名、ラベル、アドレスをラベル表に追加する
 */
bool addlabel(const char *prog, const char *label, WORD adr)
{
    assert(label != NULL);
    LABELTAB *np;
    unsigned hashval;

    /* 登録されたラベルを検索。すでに登録されている場合はエラー発生 */
    if(getlabel(prog, label) != 0xFFFF) {
        setcerr(101, label);    /* label already defined */
        return false;
    }
    /* メモリを確保 */
    np = malloc_chk(sizeof(LABELTAB), "labels.next");
    /* プログラム名を設定 */
    if(prog == NULL) {
        np->prog = NULL;
    } else {
        np->prog = strdup_chk(prog, "labels.prog");
    }
    /* ラベルを設定 */
    np->label = strdup_chk(label, "labels.label");
    /* アドレスを設定 */
    np->adr = adr;
    /* ラベル数を設定 */
    labelcnt++;
    /* ハッシュ表へ追加 */
    hashval = labelhash(prog, label);
    np->next = labels[hashval];
    labels[hashval] = np;
    return true;
}

/**
 * ラベルを比較した結果を返す
 */
int compare_adr(const void *a, const void *b)
{
    return (**(LABELARRAY **)a).adr - (**(LABELARRAY **)b).adr;
}

/**
 * ラベル表を表示する
 */
void printlabel()
{
    int i, asize = 0;
    LABELTAB *p;
    LABELARRAY *ar[labelcnt];

    for(i = 0; i < LABELTABSIZE; i++) {
        for(p = labels[i]; p != NULL; p = p->next) {
            assert(p->label != NULL);
            ar[asize] = malloc_chk(sizeof(LABELARRAY), "ar[]");
            if(p->prog == NULL) {
                ar[asize]->prog = NULL;
            } else {
                ar[asize]->prog = strdup_chk(p->prog, "ar[].prog");
            }
            ar[asize]->label = strdup_chk(p->label, "ar[].label");
            ar[asize++]->adr = p->adr;
        }
    }
    qsort(ar, asize, sizeof(*ar), compare_adr);
    for(i = 0; i < asize; i++) {
        if(ar[i]->prog != NULL) {
            fprintf(stdout, "%s.", ar[i]->prog);
        }
        fprintf(stdout, "%s ---> #%04X\n", ar[i]->label, ar[i]->adr);
    }
}

/**
 * ラベル表を解放する
 */
void freelabel()
{
    int i;
    LABELTAB *p, *q;

    for(i = 0; i < LABELTABSIZE; i++) {
        for(p = labels[i]; p != NULL; p = q) {
            q = p->next;
            FREE(p->prog);
            FREE(p->label);
            FREE(p);
        }
    }
}
