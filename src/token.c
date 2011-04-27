#include <stdio.h>
#include <string.h>
#include "cerr.h"
#include "cmem.h"
#include "assemble.h"


/**
 * 行トークン取得のエラー定義
 */
CERR cerr_linetok[] = {
    { 104, "label length is too long" },
    { 105, "no command in the line" },
};

/**
 * オペランドトークン取得のエラー定義
 */
static CERR cerr_opdtok[] = {
    { 117, "operand too many in DC" },
    { 118, "operand length too long" },
    { 121, "cannot get operand token" },
    { 123, "unclosed quote" },
};

/**
 * オペランドトークン取得のエラーを追加
 */
void addcerrlist_tok()
{
    addcerrlist(ARRAYSIZE(cerr_linetok), cerr_linetok);
    addcerrlist(ARRAYSIZE(cerr_opdtok), cerr_opdtok);
}

/**
 * 「,」区切りの文字列から、オペランドのトークンを取得
 */
OPD *opdtok(const char *str)
{
    OPD *opd = malloc_chk(sizeof(OPD), "opd");
    char *p, *q, *r, *sepp;     /* pは文字列全体の先頭位置、qはトークンの先頭位置、rは文字の位置 */
    int sepc = ',', rcnt = 0;
    bool quoting = false;

    opd->opdc = 0;
    if(str == NULL) {
        return opd;
    }
    p = q = r = strdup_chk(str, "opdtok.p");
    do {
        /* オペランド数が多すぎる場合はエラー */
        if(opd->opdc >= OPDSIZE) {
            setcerr(117, "");    /* operand is too many */
            break;
        }
        /* 先頭が等号（=）の場合 */
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
                setcerr(121, "");    /* cannot get operand token */
                break;
            }
            if(strlen(q) - rcnt > OPDSIZE) {
                setcerr(118, "");    /* operand length is too long */
                break;
            }
            opd->opdv[(++opd->opdc)-1] = strdup_chk(q, "opd.opdv[]");
            q = r = sepp + 1;
            rcnt = 0;
        }
    } while(sepc == ',');
    FREE(p);
    return opd;
}

/**
 * 空白またはタブで区切られた1行から、トークンを取得する
 */
CMDLINE *linetok(const char *line)
{
    char *tokens, *p, *sepp;
    bool quoting = false;
    CMDLINE *cmdl = NULL;

    if(*line == '\0') {
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
    if(*tokens != '\n' && *tokens != '\0') {
        p = tokens;
        cmdl = malloc_chk(sizeof(CMDLINE), "cmdl");
        cmdl->label = malloc_chk(LABELSIZE + 1, "cmdl.label");
        /* ラベルの取得。行の先頭が空白またはタブの場合、ラベルは空 */
        if((sepp = p + strcspn(p, " \t\n")) == p){
            cmdl->label = '\0';
        } else {        /* ラベルを取得 */
            *sepp = '\0';
            /* 文字列が長すぎる場合はエラー */
            if(strlen(p) > LABELSIZE) {
                setcerr(104, p);    /* label length is too long */
            }
            strcpy(cmdl->label, p);
            p = sepp + 1;
        }
        /* ラベルと命令の間の空白をスキップ */
        while(*p == ' ' || *p == '\t') {
            p++;
        }
        /* 命令とオペランドの取得 */
        if(*p == '\n' || *p == '\0') {        /* 命令がない場合は、終了 */
            if(cmdl->label != '\0') {         /* ラベルが定義されていて命令がない場合はエラー */
                setcerr(105, "");    /* no command in the line */
            }
            FREE(cmdl->label);
            FREE(cmdl);
        } else {
            /* 命令の取得 */
            sepp = p + strcspn(p, " \t\n");
            *sepp = '\0';
            cmdl->cmd = strdup_chk(p, "cmdl.cmd");
            p = sepp + 1;
            /* 命令とオペランドの間の空白をスキップ */
            while(*p == ' ' || *p == '\t') {
                p++;
            }
            /* 改行かタブまでの文字列を取得 */
            /* 「'」で囲まれた文字列に含まれる場合があるため、空白は無視 */
            if((sepp = p + strcspn(p, "\t\n")) > p) {
                *sepp = '\0';
                cmdl->opd = opdtok(p);
            } else {
                cmdl->opd = malloc_chk(sizeof(OPD), "cmdl.opd");
                cmdl->opd->opdc = 0;
            }
        }
    }
    FREE(tokens);
    return cmdl;
}
