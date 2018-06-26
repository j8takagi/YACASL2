#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "debugger.h"
#include "hash.h"
#include "cmem.h"
#include "cerr.h"
#include "exec.h"

/**
 * アドレスに対応するハッシュ値を返す
 *
 * @return ハッシュ値
 *
 * @param adr アドレス
 */
unsigned adrhash(WORD adr);

/**
 * @brief ブレークポイント数
 */
static int bpscnt = 0;

/**
 * @brief ブレークポイント表
 */
static BPSTAB *bps[BPSTABSIZE];

/**
 * @brief ブレークポイントのエラー
 */
static CERR cerr_bps[] = {
    { 101, "break point already defined" },
    { 102, "break point table is full" },
    { 103, "break point not found" },
};

/**
 * @brief アドレスのハッシュ値を返す
 *
 * @return ハッシュ値
 *
 * @param adr アドレス
 */
unsigned adrhash(WORD adr);

/**
 * @brief ブレークポイントのエラーをエラーリストに追加する
 *
 * @return なし
 */
void addcerrlist_bps();


unsigned adrhash(WORD adr)
{
    HKEY *key[1];
    unsigned h;
    key[0] = malloc_chk(sizeof(HKEY), "adrhash.key");
    key[0]->type = INT;
    key[0]->val.i = adr;
    h = hash(1, key, BPSTABSIZE);
    FREE(key[0]);
    return h;
}

void addcerrlist_bps()
{
    addcerrlist(ARRAYSIZE(cerr_bps), cerr_bps);
}

bool getbps(WORD adr)
{
    BPSTAB *p;

    for(p = bps[adrhash(adr)]; p != NULL; p = p->next) {
        if(p->adr == adr) {
            return true;
        }
    }
    return false;
}

bool addbps(WORD adr)
{
    BPSTAB *p;
    unsigned h;

    /* 登録されたラベルを検索。すでに登録されている場合はエラー発生 */
    if(getbps(adr) == true) {
        setcerr(101, "");    /* breakpoint already defined */
        return false;
    }
    /* メモリを確保 */
    p = malloc_chk(sizeof(BPSTAB), "bps.next");
    /* アドレスを設定 */
    p->adr = adr;
    /* ブレークポイント数を設定 */
    bpscnt++;
    /* ハッシュ表へ追加 */
    p->next = bps[h = adrhash(adr)];
    bps[h] = p;
    return true;
}

bool delbps(WORD adr)
{
    BPSTAB *p, *q;
    unsigned h;

    p = bps[h = adrhash(adr)];
    if(p->adr == adr && p->next != NULL) {
        bps[h] = p->next;
        FREE(p);
        return true;
    } else {
        for(; p != NULL; p = p->next) {
            if(p->adr == adr) {
                q = p->next;
                p->next = q->next;
                FREE(p);
                return true;
            }
        }
    }
    return false;
}

void printbps()
{
    int i;
    BPSTAB *p;

    fprintf(stdout, "breakpoints list\n");
    for(i = 0; i < BPSTABSIZE; i++) {
        for(p = bps[i]; p != NULL; p = p->next) {
            fprintf(stdout, "#%04X\n", p->adr);
        }
    }
}

void freebps()
{
    int i;
    BPSTAB *p, *q;

    for(i = 0; i < BPSTABSIZE; i++) {
        for(p = bps[i]; p != NULL; p = q) {
            q = p->next;
            FREE(p);
        }
    }
}

DBARGS *dbargstok(const char *str)
{
    DBARGS *args = malloc_chk(sizeof(DBARGS), "args");
    char *p, *q, *r, *sepp;     /* pは文字列全体の先頭位置、qはトークンの先頭位置、rは文字の位置 */
    int sepc = ' ';

    args->argc = 0;
    if(str == NULL) {
        return args;
    }
    p = q = r = strdup_chk(str, "argstok.p");
    do {
        while(*q == ' ' || *q == '\t'){
            q = ++r;
        }
        sepp = r + strcspn(r, " ");
        sepc = *sepp;
        *sepp = '\0';
        args->argv[(++args->argc)-1] = strdup_chk(q, "args.argv[]");
        q = r = sepp + 1;
    } while(sepc == ' ');
    FREE(p);
    return args;
}

DBCMDLINE *dblinetok(const char *line)
{
    char *tokens, *p, *sepp;
    DBCMDLINE *dbcmdl = NULL;

    if(*line == '\0') {
        return NULL;
    }
    tokens = strdup_chk(line, "tokens");
    p = tokens;
    dbcmdl = malloc_chk(sizeof(DBCMDLINE), "dbcmdl");
    /* コマンドと引数の取得 */
    if(*p == '\n' || *p == '\0') {        /* コマンドがない場合は、終了 */
        dbcmdl->dbcmd = '\0';
    } else {
        /* コマンドの取得 */
        sepp = p + strcspn(p, " \t\n");
        *sepp = '\0';
        dbcmdl->dbcmd = strdup_chk(p, "dbcmdl.dbcmd");
        p = sepp + 1;
        /* コマンドと引数の間の空白をスキップ */
        while(*p == ' ' || *p == '\t') {
            p++;
        }
        /* 改行までの文字列を取得 */
        if((sepp = p + strcspn(p, "\n")) > p) {
            *sepp = '\0';
            dbcmdl->dbargs = dbargstok(p);
        } else {
            dbcmdl->dbargs = malloc_chk(sizeof(DBARGS), "dbcmdl.dbargs");
            dbcmdl->dbargs->argc = 0;
        }
    }
    FREE(tokens);
    return dbcmdl;
}

bool stracmp(char *str1, int str2c, char *str2v[])
{
    int i;
    for(i = 0; i < str2c; i++) {
        if(strcmp(str1, str2v[i]) == 0) {
            return true;
        }
    }
    return false;
}

void debugger()
{
    char *buf = malloc_chk(DBINSIZE + 1, "debugger.buf"), *cmd;
    DBCMDLINE *dbcmdl =  malloc_chk(sizeof(DBCMDLINE), "dbcmdl");
    DBARGS *arg;
    for( ; ;) {
        fprintf(stdout, "COMET II (Type ? for help) > ");
        fgets(buf, DBINSIZE, stdin);
        dbcmdl = dblinetok(buf);
        cmd = dbcmdl->dbcmd;
        arg = dbcmdl->dbargs;
        if(*buf == '\n' || stracmp(cmd, 2, (char* []){"s", "step"})) {
            break;
        } else if(stracmp(cmd, 2, (char* []){"c", "continue"})) {
            execmode.debugger = false;
            break;
        } else if(stracmp(cmd, 2, (char* []){"t", "trace"})) {
            if(arg->argc > 0 && stracmp(arg->argv[0], 2, (char* []){"a", "auto"})) {
                execmode.logical = false;
                execmode.trace = true;
            } else if(arg->argc > 0 && stracmp(arg->argv[0], 2, (char* []){"no", "noauto"})) {
                execmode.trace = false;
            } else {
                fprintf(stdout, "#%04X: Register::::\n", sys->cpu->pr);
                dspregister();
            }
        } else if(stracmp(cmd, 2, (char* []){"T", "tracelogical"})) {
            if(arg->argc > 0 && stracmp(arg->argv[0], 2, (char* []){"a", "auto"})) {
                execmode.logical = true;
                execmode.trace = true;
            } else if(arg->argc > 0 && stracmp(arg->argv[0], 2, (char* []){"no", "noauto"})) {
                execmode.trace = false;
            } else {
                fprintf(stdout, "#%04X: Register::::\n", sys->cpu->pr);
                dspregister();
            }
        } else if(stracmp(cmd, 2, (char* []){"d", "dump"})) {
            if(arg->argc > 0 && stracmp(arg->argv[0], 2, (char* []){"a", "auto"})) {
                execmode.dump = true;
            } else if(arg->argc > 0 && stracmp(arg->argv[0], 2, (char* []){"no", "noauto"})) {
                execmode.dump = false;
            } else {
                dumpmemory();
            }
        } else if(*buf == '?') {
            fprintf(stdout, "s (default) -- Step by step running your program until next interaction.\n");
            fprintf(stdout, "c -- Continue running your program.\n");
            fprintf(stdout, "t -- Display CPU register.\n");
            fprintf(stdout, "d -- Display memory dump.\n");
            break;
        }
    }
}
