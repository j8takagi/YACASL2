#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cerr.h"
#include "cmem.h"
#include "hash.h"
#include "assemble.h"

/**
 * プログラム名とラベルに対応するハッシュ値を返す
 *
 * @return ハッシュ値
 *
 * @param prog プログラム名
 * @param label ラベル
 */
unsigned labelhash(const char *prog, const char *label);

/**
 * ラベルを比較した結果を返す。qsort内で使われる関数
 *
 * @return ラベルが同一の場合は0、異なる場合は0以外
 *
 * @param *a ラベルa
 * @param *b ラベルb
 */
int compare_adr(const void *a, const void *b);

/**
 * @brief ラベル数
 */
static int labelcnt = 0;

/**
 * @brief ラベル表
 */
static LABELTAB *labels[LABELTABSIZE];

/**
 * @brief ラベルのエラー
 */
static CERR cerr_label[] = {
    { 101, "label already defined" },
    { 102, "label table is full" },
    { 103, "label not found" },
};

unsigned labelhash(const char *prog, const char *label)
{
    HKEY *keys[2];
    int i = 0, j;
    unsigned h;

    if(*prog != '\0') {
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

int compare_adr(const void *a, const void *b)
{
    return (**(LABELARRAY **)a).adr - (**(LABELARRAY **)b).adr;
}

/* assemble.hで定義された関数群 */
void addcerrlist_label()
{
    addcerrlist(ARRAYSIZE(cerr_label), cerr_label);
}

WORD getlabel(const char *prog, const char *label)
{
    assert(prog != NULL && label != NULL);
    LABELTAB *p;

    for(p = labels[labelhash(prog, label)]; p != NULL; p = p->next) {
        if((*prog == '\0' || (strcmp(prog, p->prog) == 0)) &&
           strcmp(label, p->label) == 0)
        {
            return p->adr;
        }
    }
    return 0xFFFF;
}

bool addlabel(const char *prog, const char *label, WORD adr)
{
    assert(label != NULL);
    LABELTAB *p;
    unsigned hashval;

    /* 登録されたラベルを検索。すでに登録されている場合はエラー発生 */
    if(getlabel(prog, label) != 0xFFFF) {
        setcerr(101, label);    /* label already defined */
        return false;
    }
    /* メモリを確保 */
    p = malloc_chk(sizeof(LABELTAB), "labels.next");
    /* プログラム名を設定 */
    p->prog = strdup_chk(prog, "labels.prog");
    /* ラベルを設定 */
    p->label = strdup_chk(label, "labels.label");
    /* アドレスを設定 */
    p->adr = adr;
    /* ラベル数を設定 */
    labelcnt++;
    /* ハッシュ表へ追加 */
    hashval = labelhash(prog, label);
    p->next = labels[hashval];
    labels[hashval] = p;
    return true;
}

void printlabel()
{
    int i, s = 0;
    LABELTAB *p;
    LABELARRAY **l;

    l = calloc_chk(labelcnt, sizeof(LABELARRAY **), "labels");
    for(i = 0; i < LABELTABSIZE; i++) {
        for(p = labels[i]; p != NULL; p = p->next) {
            assert(p->label != NULL);
            l[s] = malloc_chk(sizeof(LABELARRAY), "lables");
            l[s]->prog = strdup_chk(p->prog, "labels.prog");
            l[s]->label = strdup_chk(p->label, "labels.label");
            l[s++]->adr = p->adr;
        }
    }
    qsort(l, s, sizeof(*l), compare_adr);
    for(i = 0; i < s; i++) {
        if(*(l[i]->prog) != '\0') {
            fprintf(stdout, "%s.", l[i]->prog);
        }
        fprintf(stdout, "%s ---> #%04X\n", l[i]->label, l[i]->adr);
        FREE(l[i]->prog);
        FREE(l[i]->label);
        FREE(l[i]);
    }
    FREE(l);
}

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
