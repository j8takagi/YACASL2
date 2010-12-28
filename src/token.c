#include <stdio.h>
#include <string.h>
#include "cerr.h"
#include "cmem.h"
#include "assemble.h"

/**
 * 「,」区切りの文字列から、オペランドのトークンを取得
 */
OPD *opdtok(const char *str)
{
    OPD *opd = malloc_chk(sizeof(OPD), "opd");
    char *p, *q, *r, *sepp;     /* pは文字列全体の先頭位置、qはトークンの先頭位置、rは文字の位置 */
    int sepc = ',', rcnt = 0;
    bool quoting = false;

    CERR cerr_opdtok[] = {
        { 117, "operand too many in DC" },
        { 118, "operand length too long" },
        { 121, "cannot get operand token" },
        { 123, "unclosed quote" },
    };
    addcerrlist(ARRAYSIZE(cerr_opdtok), cerr_opdtok);
    opd->opdc = 0;
    if(str == NULL) {
        return opd;
    }
    p = q = r = strdup_chk(str, "opdtopk.p");
    do {
        /* オペランド数が多すぎる場合はエラー */
        if(opd->opdc >= OPDSIZE) {
            setcerr(117, NULL);    /* operand is too many */
            break;
        }
        /* 先頭が「=」の場合 */
        if(*r == '=') {
            r++;
        }
        /* 「'」の場合 */
        if(*r == '\'') {
            /* 「''」以外の場合はquote値を反転 */
            if(*(r+1) != '\'' && !(q < r && *(r-1) == '\'')) {
                quoting = !quoting;
            }
            /* 文字列の長さを数える。「'」の場合は数えない */
            if(*(r+1) != '\'') {
                rcnt++;
            }
        }
        if(quoting == true) {
            /* 閉じ「'」がないまま文字列が終了した場合 */
            if(*r == '\0') {
                setcerr(123, str);    /* unclosed quote */
                break;
            }
            r++;
        } else {
            sepp = r + strcspn(r, ", ");
            sepc = *sepp;
            *sepp = '\0';
            if(*q == '\0') {
                setcerr(121, NULL);    /* cannot get operand token */
                break;
            }
            if(strlen(q) - rcnt > OPDSIZE) {
                setcerr(118, NULL);    /* operand length is too long */
                break;
            }
            opd->opdv[(++opd->opdc)-1] = strdup_chk(q, "opd.opdv[]");
            q = r = sepp + 1;
            rcnt = 0;
        }
    } while(sepc == ',');
    free_chk(p, "opdtok.p");
    return opd;
}

/**
 * 空白またはタブで区切られた1行から、トークンを取得する
 */
CMDLINE *linetok(const char *line)
{
    char *tokens, *p, *sepp;
    bool quoting = false;
    CMDLINE *cmdl = malloc_chk(sizeof(CMDLINE), "cmdl");

    CERR cerr_linetok[] = {
        { 104, "label length is too long" },
        { 105, "no command in the line" },
    };
    addcerrlist(ARRAYSIZE(cerr_linetok), cerr_linetok);
    if(line == NULL || strlen(line) == 0) {
        return NULL;
    }
    tokens = strdup_chk(line, "tokens");
    /* コメントを削除 */
    for(p = tokens; *p != '\0'; p++) {
        /* 「'」で囲まれた文字列の処理。「''」は無視 */
        if(*p == '\'' && *(p+1) != '\'' && !(p > tokens && *(p-1) == '\'')) {
            quoting = !quoting;
        } else if(quoting == false && *p == ';') {
            *p = '\0';
            break;
        }
    }
    if(*tokens == '\0') {
        return NULL;
    }
    p = tokens;
    /* 行の先頭が空白またはタブの場合、ラベルは空 */
    if((sepp = p + strcspn(p, " \t\n")) == p){
        cmdl->label = NULL;
    } else {        /* ラベルを取得 */
        *sepp = '\0';
        /* 文字列が長すぎる場合はエラー */
        if(strlen(p) > LABELSIZE) {
            setcerr(104, p);    /* label length is too long */
        }
        cmdl->label = strdup_chk(p, "cmdl.label");
        p = sepp + 1;
    }
    while(*p == ' ' || *p == '\t') {
        p++;
    }
    /* 命令がない場合、終了 */
    if(*p == '\n' || *p == '\0') {
        /* ラベルが定義されていて命令がない場合はエラー */
        if(cmdl->label != NULL) {
            setcerr(105, NULL);    /* no command in the line */
        }
        return NULL;
    }
    /* 命令を取得 */
    sepp = p + strcspn(p, " \t\n");
    *sepp = '\0';
    cmdl->cmd = strdup_chk(p, "cmdl.cmd");
    p = sepp + 1;
    while(*p == ' ' || *p == '\t') {
        p++;
    }
    /* オペランドを取得 */
    cmdl->opd = malloc_chk(sizeof(OPD), "cmdl.opd");
    /* 改行かタブまでの文字列を取得。
       「'」で囲まれた文字列に含まれる場合があるため、空白は無視 */
    if((sepp = p + strcspn(p, "\t\n")) > p) {
        *sepp = '\0';
        cmdl->opd = opdtok(p);
    } else {
        cmdl->opd->opdc = 0;
    }
    return cmdl;
}
