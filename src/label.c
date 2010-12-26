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

#ifndef UNITTEST
static unsigned labelhash(const char *prog, const char *label);

static int compare_adr(const void *a, const void *b);
#endif

/**
 * プログラム名とラベルに対応するハッシュ値を返す
 */
unsigned labelhash(const char *prog, const char *label)
{
    HKEY *keys[2];
    int i = 0;

    if(prog != NULL) {
        keys[i] = malloc_chk(sizeof(HKEY), "labelhash.key[]");
        keys[i]->type = CHARS;
        keys[i]->val.s = strdup_chk(prog, "labelhash.key[].val");
    }
    keys[i] = malloc_chk(sizeof(HKEY), "labelhash.key[]");
    keys[i]->type = CHARS;
    keys[i]->val.s = strdup_chk(label, "labelhash.key[].val");
    /* ハッシュ値を返す */
    return hash(i+1, keys, LABELTABSIZE);
}

/**
 * ラベル表からアドレスを検索する
 */
WORD getlabel(const char *prog, const char *label)
{
    assert(label != NULL);
    LABELTAB *np;

    for(np = labels[labelhash(prog, label)]; np != NULL; np = np->next) {
        if((prog == NULL || (np->prog != NULL && strcmp(prog, np->prog) == 0)) &&
           strcmp(label, np->label) == 0)
        {
            return np->adr;
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
    LABELTAB *np;
    LABELARRAY *ar[labelcnt];

    for(i = 0; i < LABELTABSIZE; i++) {
        for(np = labels[i]; np != NULL; np = np->next) {
            assert(np->label != NULL);
            ar[asize] = malloc_chk(sizeof(LABELARRAY), "ar[]");
            if(np->prog == NULL) {
                ar[asize]->prog = NULL;
            } else {
                ar[asize]->prog = strdup_chk(np->prog, "ar[].prog");
            }
            ar[asize]->label = strdup_chk(np->label, "ar[].label");
            ar[asize++]->adr = np->adr;
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
    LABELTAB *np, *nq;

    for(i = 0; i < LABELTABSIZE; i++) {
        for(np = labels[i]; np != NULL; np = nq) {
            nq = np->next;
            if(np->prog != NULL) {
                free_chk(np->prog, "np.prog");
            }
            free_chk(np->label, "np.label");
            free_chk(np, "np");
        }
    }
}
