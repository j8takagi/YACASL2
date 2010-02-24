#include "casl2.h"
#include "assemble.h"

int labelcnt = 0;                    /* ラベル数 */
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

/* ラベルを表に追加する */
bool addlabel(const char *prog, const char *label, WORD adr)
{
    LABELTAB *np;
    unsigned hashval;
    char *keys[2];
    int i = 0;

    if(getlabel(prog, label) != 0xFFFF) {
        setcerr(101, label);    /* label already defined */
        return false;
    }
    np = malloc(sizeof(*np));
    if(np == NULL || (np->label = strdup(label)) == NULL ||
       (prog != NULL && (np->prog = strdup(prog)) == NULL))
    {
        setcerr(102, NULL);    /* label table is full */
        return false;
    }
    if(prog != NULL) {
        keys[i++] = strdup(prog);
    }
    labelcnt++;
    keys[i] = strdup(label);
    hashval = labelhash(prog, label);
    np->next = labels[hashval];
    labels[hashval] = np;
    np->adr = adr;
    return true;
}

int compare_adr(const void *a, const void *b)
{
    return (**(const LABELARRAY **)a).adr - (**(const LABELARRAY **)b).adr;
}

/* ラベル表を表示する */
void printlabel()
{
    int i, asize = 0;
    LABELTAB *np = malloc(sizeof(LABELTAB *));
    LABELARRAY **ar = malloc(labelcnt * sizeof(LABELARRAY **));

    for(i = 0; i < LABELTABSIZE; i++) {
        for(np = labels[i]; np != NULL; np = np->next) {
            ar[asize] = malloc(sizeof(LABELARRAY *));
            ar[asize]->prog = (np->prog == NULL ? NULL : strdup(np->prog));
            ar[asize]->label = strdup(np->label);
            ar[asize]->adr = np->adr;
            asize++;
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
            free(np->prog);
            free(np->label);
            free(np);
        }
    }
}
