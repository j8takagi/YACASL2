#include "casl2.h"
#include "assemble.h"

/* 「,」区切りの文字列から、オペランドのトークンを取得 */
OPD *opdtok(const char *str)
{
    OPD *opd = malloc(sizeof(OPD));
    char *p, *q, *sepp;
    int sepc = ',';
    bool quoting = false;

    opd->opdc = 0;
    if(str == NULL) {
        return opd;
    }
    p = q = strdup(str);
    do {
        /* オペランド数が多すぎる場合はエラー */
        if(opd->opdc >= OPDSIZE) {
            setcerr(117, str);    /* operand is too many */
            break;
        }
        /* 先頭が「=」の場合の処理 */
        if(*q == '=') {
            q++;
        }
        /* 「'」で囲まれた文字列の処理。「''」は無視 */
        if(*q == '\'' && *(q+1) != '\'' && !(p < q && *(q-1) == '\'')) {
            quoting = !quoting;
        }
        if(quoting == true) {
            /* 閉じ「'」がないまま文字列が終了した場合 */
            if(*q == '\0') {
                setcerr(123, str);    /* illegal string */
                break;
            }
            q++;
        } else {
            sepp = q + strcspn(q, ", ");
            sepc = *sepp;
            *sepp = '\0';
            if(strlen(p) == 0) {
                setcerr(121, NULL);    /* cannot get operand token */
                break;
            }
            if(strlen(p) > OPDSIZE + 2) {    /* OPDSIZE + 「'」2文字分 */
                setcerr(118, p);    /* operand length is too long */
                break;
            }
            opd->opdv[(++opd->opdc)-1] = strdup(p);
            p = q = sepp + 1;
        }
    } while(sepc == ',');
    return opd;
}

/* 空白またはタブで区切られた1行から、トークンを取得 */
CMDLINE *linetok(const char *line)
{
    char *tokens, *p, *sepp;
    bool quoting = false;
    CMDLINE *cmdl = malloc(sizeof(CMDLINE));

    if(line == NULL || strlen(line) == 0) {
        return NULL;
    }
    tokens = strdup(line);
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
        cmdl->label = strdup(p);
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
    cmdl->cmd = strdup(p);
    p = sepp + 1;
    while(*p == ' ' || *p == '\t') {
        p++;
    }
    /* オペランドを取得 */
    cmdl->opd = malloc(sizeof(OPD));
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
