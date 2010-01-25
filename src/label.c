#include "casl2.h"
#include "assemble.h"

LABELTAB *labels[LABELTABSIZE];

/* ラベル表からアドレスを検索する */
WORD getlabel(const char *label, const char *prog)
{
    LABELTAB *np;

    for(np = labels[hash(label, LABELTABSIZE)]; np != NULL; np = np->next) {
        if(strcmp(label, np->label) == 0) {
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

    if(getlabel(label, prog) != 0xFFFF) {
        setcerr(101, label);    /* label already defined */
        return false;
    }
    np = (LABELTAB *) malloc(sizeof(*np));
    if(np == NULL || (np->label = strdup(label)) == NULL ||
       (prog != NULL && (np->prog = strdup(prog)) == NULL))
    {
        setcerr(102, NULL);    /* label table is full */
        return false;
    }
    hashval = hash(label, LABELTABSIZE);
    np->next = labels[hashval];
    labels[hashval] = np;
    np->adr = adr;
    return true;
}

/* ラベル表を表示する */
void printlabel()
{
    int i;
    LABELTAB *np;
    for(i = 0; i < LABELTABSIZE; i++) {
        for(np = labels[i]; np != NULL; np = np->next) {
            if(np->prog == NULL) {
                fprintf(stdout, "%s ---> #%04X\n", np->label, np->adr);
            } else {
                fprintf(stdout, "%s.%s ---> #%04X\n", np->prog, np->label, np->adr);
            }
        }
    }
}

/* ラベル表を解放する */
void freelabel()
{
    int i;
    LABELTAB *np, *nq;
    for(i = 0; i < LABELTABSIZE; i++) {
        for(np = labels[i]; np != NULL; np = nq){
            nq = np->next;
            free(np->prog);
            free(np->label);
            free(np);
        }
    }
}
