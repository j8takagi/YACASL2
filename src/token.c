#include <stdio.h>
#include <string.h>
#include "cerr.h"
#include "cmem.h"
#include "assemble.h"

/**
 * @brief 「,」区切りの文字列から、オペランドのトークンを取得
 *
 * @return オペランドのトークン
 *
 * @param *str 「,」区切りの文字列
 */
OPD *opdtok(const char *str);

/**
 * @brief 行トークン取得のエラー定義
 */
CERR cerr_linetok[] = {
    { 104, "label length is too long" },
    { 105, "no command in the line" },
};

/**
 * @brief オペランドトークン取得のエラー定義
 */
static CERR cerr_opdtok[] = {
    { 117, "operand too many in DC" },
    { 118, "operand length too long" },
    { 121, "cannot get operand token" },
    { 123, "unclosed quote" },
};

OPD *opdtok(const char *str)
{
    OPD *opd = malloc_chk(sizeof(OPD), "opd");
    char *tok, *p, sepc = ',';
    int i = 0, cnt_quote = 0;
    bool quoting = false;

    opd->opdc = 0;
    if(!str || !str[0]) {
        return opd;
    }
    tok = p = strdup_chk(str, "opdtok.p");
    do {
        /* オペランド数が多すぎる場合はエラー */
        if(opd->opdc >= OPDSIZE) {
            setcerr(117, "");    /* operand is too many */
            break;
        }
        /* 先頭が等号（=）の場合 */
        if(p[i] == '=') {
            i++;
        }
        /* 「'」の場合 */
        if(p[i] == '\'') {
            /* 「''」以外の場合はquote値を反転する */
            if(p[i+1] != '\'' && (i == 0 || p[i-1] != '\'')) {
                quoting = !quoting;
            }
            /* 「'」をカウントする。「''」の場合は1をカウント */
            if(p[i+1] != '\'') {
                cnt_quote++;
            }
        }
        if(quoting == true) {
            /* 「'」で開いたまま文字列が終了した場合 */
            if(!p[i]) {
                setcerr(123, str);    /* unclosed quote */
                break;
            }
            i++;
        } else {
            i += strcspn(p + i, ", ");
            if(i == 0) {
                setcerr(121, "");    /* cannot get operand token */
                break;
            }
            if(i - cnt_quote > OPDSIZE) {
                setcerr(118, "");    /* operand length too long */
                break;
            }
            sepc = p[i];
            opd->opdv[(opd->opdc)++] = strndup_chk(p, i, "opd->opdv[]");
            p += i + 1;
            i = cnt_quote = 0;
        }
    } while(sepc == ',');
    FREE(tok);
    return opd;
}

/* assemble.hで定義された関数群 */
void addcerrlist_tok()
{
    addcerrlist(ARRAYSIZE(cerr_linetok), cerr_linetok);
    addcerrlist(ARRAYSIZE(cerr_opdtok), cerr_opdtok);
}

char *strip_casl2_comment(char *s)
{
    int i;
    bool quoting = false;

    for(i = 0; s[i]; i++) {
        /* 「'」で囲まれた文字列の場合。「''」は無視 */
        if(s[i] == '\'' && s[i+1] != '\'' && (i == 0 || s[i-1] != '\'')) {
            quoting = !quoting;
        /* 「'」で囲まれた文字列でない場合、文字列末尾の「;」以降を削除 */
        } else if(quoting == false && s[i] == ';') {
            s[i] = '\0';
            break;
        }
    }
    return s;
}

CMDLINE *linetok(const char *line)
{
    char *tok = NULL, *p = NULL, *lbl = NULL;
    int i;
    CMDLINE *cmdl = NULL;

    if(!line[0] || line[0] == '\n') {
        return NULL;
    }
    tok = p = strdup_chk(line, "tok");
    /* コメントを削除 */
    strip_casl2_comment(p);
    /* 文字列末尾の改行と空白を削除 */
    strip_end(p);
    /* 空行の場合、終了 */
    if(!p[0]) {
        goto linetokfin;
    }
    cmdl = malloc_chk(sizeof(CMDLINE), "cmdl");

    /* ラベルの取得 */
    /* 行の先頭が空白またはタブの場合、ラベルは空 */
    if((i = strcspn(p, " \t")) == 0) {
        lbl = strdup_chk("", "linetok.lbl");
    } else {
        lbl = strndup_chk(p, i, "linetok.lbl");
        /* ラベルの文字列が長すぎる場合はエラー */
        if(i > LABELSIZE) {
            setcerr(104, lbl);    /* label length is too long */
            FREE(lbl);
            goto linetokfin;
        }
        /* 文字列先頭をラベルの次の文字に移動 */
        p += i;
    }
    /* ラベル取得の実行 */
    cmdl->label = lbl;

    /* 命令の取得 */
    /* 文字列先頭を、ラベルと命令の間の空白の後ろに移動 */
    p += strspn(p, " \t");
    /* 命令がない場合は、終了 */
    if(!p[0]) {
        if(cmdl->label) {      /* ラベルが定義されていて命令がない場合はエラー */
            setcerr(105, "");    /* no command in the line */
        }
        FREE(cmdl->label);
        FREE(cmdl);
        goto linetokfin;
    }
    /* 命令取得の実行 */
    i = strcspn(p, " \t");
    cmdl->cmd = strndup_chk(p, i, "cmdl.cmd");

    /* オペランドの取得 */
    /* 文字列先頭を、命令の次の文字に移動 */
    p += i;
    /* 文字列先頭を、命令とオペランドの間の空白の後ろに移動 */
    p += strspn(p, " \t");
    /* オペランド取得の実行 */
    cmdl->opd = opdtok(p);
linetokfin:
    FREE(tok);
    return cmdl;
}
