#include "casl2.h"
#include "assemble.h"

int labelcnt = 0;                /* ラベル数 */
LABELTAB *labels[LABELTABSIZE];  /* ラベル表 */

/* プログラム名とラベルに対応するハッシュ値を返す */
unsigned labelhash(const char *prog, const char *label)
{
    HKEY *keys[2];
    int i = 0;
    if(prog != NULL) {
        keys[i] = malloc(sizeof(HKEY));
        keys[i]->type = CHARS;
        keys[i++]->val.s = strdup(prog);
    }
    keys[i] = malloc(sizeof(HKEY));
    keys[i]->type = CHARS;
    keys[i]->val.s = strdup(label);
    /* ハッシュ値を返す */
    return hash(i+1, keys, LABELTABSIZE);
}

/* ラベル表からアドレスを検索する */
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

/* プログラム名、ラベル、アドレスをラベル表に追加する */
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
    if((np = malloc(sizeof(LABELTAB))) == NULL) {
        goto cerr102;
    }
    /* プログラム名を設定 */
    if(prog == NULL) {
        np->prog = NULL;
    } else {
        if((np->prog = strdup(prog)) == NULL) {
            goto cerr102;
        }
    }
    /* ラベルを設定 */
    if((np->label = strdup(label)) == NULL) {
        goto cerr102;
    }
    /* アドレスを設定 */
    np->adr = adr;
    /* ラベル数を設定 */
    labelcnt++;
    /* ハッシュ表へ追加 */
    hashval = labelhash(prog, label);
    np->next = labels[hashval];
    labels[hashval] = np;
    return true;
cerr102:
    setcerr(102, NULL);    /* label table is full */
    return false;
}

int compare_adr(const void *a, const void *b)
{
    return (**(LABELARRAY **)a).adr - (**(LABELARRAY **)b).adr;
}

/* ラベル表を表示する */
void printlabel()
{
    int i, asize = 0;
    LABELTAB *np;
    LABELARRAY *ar[labelcnt];

    for(i = 0; i < LABELTABSIZE; i++) {
        for(np = labels[i]; np != NULL; np = np->next) {
            assert(np->label != NULL);
            ar[asize] = malloc(sizeof(LABELARRAY));
            ar[asize]->prog = (np->prog == NULL ? NULL : strdup(np->prog));
            ar[asize]->label = strdup(np->label);
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

/* ラベル表を解放する */
void freelabel()
{
    int i;
    LABELTAB *np, *nq;
    for(i = 0; i < LABELTABSIZE; i++) {
        for(np = labels[i]; np != NULL; np = nq) {
            nq = np->next;
            if(np->prog != NULL) {
                free(np->prog);
            }
            free(np->label);
            free(np);
        }
    }
}
