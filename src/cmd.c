#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "hash.h"
#include "struct.h"
#include "cmem.h"
#include "exec.h"

/**
 * 機械語命令のリスト
 */
static CMD comet2cmd[] = {
    { "NOP", NONE, 0x0, nop },
    { "LD", R_ADR_X_, 0x1000, ld },
    { "ST", R_ADR_X, 0x1100, st },
    { "LAD", R_ADR_X, 0x1200, lad },
    { "LD", R1_R2, 0x1400, ld },
    { "ADDA", R_ADR_X_, 0x2000, adda },
    { "SUBA", R_ADR_X_, 0x2100, suba },
    { "ADDL", R_ADR_X_, 0x2200, addl },
    { "SUBL", R_ADR_X_, 0x2300, subl },
    { "ADDA", R1_R2, 0x2400, adda },
    { "SUBA", R1_R2, 0x2500, suba },
    { "ADDL", R1_R2, 0x2600, addl },
    { "SUBL", R1_R2, 0x2700, subl },
    { "AND", R_ADR_X_, 0x3000, and },
    { "OR", R_ADR_X_, 0x3100, or },
    { "XOR", R_ADR_X_, 0x3200, xor },
    { "AND", R1_R2, 0x3400, and },
    { "OR", R1_R2, 0x3500, or },
    { "XOR", R1_R2, 0x3600, xor },
    { "CPA", R_ADR_X_, 0x4000, cpa },
    { "CPL", R_ADR_X_, 0x4100, cpl },
    { "CPA", R1_R2, 0x4400, cpa },
    { "CPL", R1_R2, 0x4500, cpl },
    { "SLA", R_ADR_X, 0x5000, sla },
    { "SRA", R_ADR_X, 0x5100, sra },
    { "SLL", R_ADR_X, 0x5200, sll },
    { "SRL", R_ADR_X, 0x5300, srl },
    { "JMI", ADR_X, 0x6100, jmi },
    { "JNZ", ADR_X, 0x6200, jnz },
    { "JZE", ADR_X, 0x6300, jze },
    { "JUMP", ADR_X, 0x6400, jump },
    { "JPL", ADR_X, 0x6500, jpl },
    { "JOV", ADR_X, 0x6600, jov },
    { "PUSH", ADR_X, 0x7000, push },
    { "POP", R_, 0x7100, pop },
    { "CALL", ADR_X, 0x8000, call },
    { "SVC", ADR_X, 0xF000, svc },
    { "RET", NONE, 0x8100, ret },
};

/**
 * 命令コード配列のサイズ
 */
static int comet2cmdsize = ARRAYSIZE(comet2cmd);

/**
 * ハッシュ表のサイズ
 */
enum {
    CMDTABSIZE = 39,
};

/**
 * ハッシュ表
 */
static CMDTAB *cmdtype_code[CMDTABSIZE], *code_type[CMDTABSIZE];

/**
 * 命令の名前とタイプからハッシュ値を生成する
 */
unsigned hash_cmdtype(const char *cmd, CMDTYPE type)
{
    HKEY *keys[2];
    unsigned hashval;

    /* 命令名を設定 */
    keys[0] = malloc_chk(sizeof(HKEY), "hash_cmdtype.keys[0]");
    keys[0]->type = CHARS;
    keys[0]->val.s = strdup_chk(cmd, "keys[0].val.s");
    /* 命令タイプを設定 */
    keys[1] = malloc_chk(sizeof(HKEY), "hash_cmdtype.keys[1]");
    keys[1]->type = INT;
    keys[1]->val.i = (int)(type & 070);
    /* ハッシュ値の計算 */
    hashval = hash(2, keys, CMDTABSIZE);
    FREE(keys[0]->val.s);
    FREE(keys[0]);
    FREE(keys[1]);
    /* ハッシュ値を返す */
    return hashval;
}

/**
 * 名前とタイプがキーの命令ハッシュ表を作成する
 */
bool create_cmdtype_code()
{
    CMDTAB *p;
    unsigned hashval;
    int i;

    for(i = 0; i < comet2cmdsize; i++) {
        hashval = hash_cmdtype(comet2cmd[i].name, comet2cmd[i].type);    /* ハッシュ値の生成 */
        p = malloc_chk(sizeof(CMDTAB), "cmdtype_code");
        p->cmd = &comet2cmd[i];
        p->next = cmdtype_code[hashval];                                 /* ハッシュ表に値を追加 */
        cmdtype_code[hashval] = p;
    }
    return true;
}

/**
 * 命令の名前とタイプから、命令コードを返す
 * 無効な場合は0xFFFFを返す
 */
WORD getcmdcode(const char *cmd, CMDTYPE type)
{
    CMDTAB *p;
    WORD w = 0xFFFF;

    assert(cmd != NULL);
    for(p = cmdtype_code[hash_cmdtype(cmd, type)]; p != NULL; p = p->next) {
        if(strcmp(cmd, p->cmd->name) == 0 && type == p->cmd->type) {
            w = p->cmd->code;
            break;
        }
    }
    return w;
}

/**
 * 名前とタイプがキーの命令ハッシュ表を解放する
 */
void free_cmdtype_code()
{
    int i;
    CMDTAB *p, *q;

    for(i = 0; i < CMDTABSIZE; i++) {
        for(p = cmdtype_code[i]; p != NULL; p = q) {
            q = p->next;
            FREE(p);
        }
    }
}

/**
 * 命令コードからハッシュ値を生成する
 */
unsigned hash_code(WORD code)
{
    HKEY *keys[1];
    unsigned h;

    /* 命令コードを設定 */
    keys[0] = malloc_chk(sizeof(HKEY), "hash_code.key");
    keys[0]->type = INT;
    keys[0]->val.i = (int)(code >> 8);
    h = hash(1, keys, CMDTABSIZE);
    FREE(keys[0]);
    return h;
}

/**
 * コードがキーの命令ハッシュ表を作成する
 */
bool create_code_type()
{
    CMDTAB *p;
    unsigned hashval;
    int i;

    for(i = 0; i < comet2cmdsize; i++) {
        hashval = hash_code((&comet2cmd[i])->code);    /* ハッシュ値の生成 */
        p = malloc_chk(sizeof(CMDTAB), "code_type");
        p->cmd = &comet2cmd[i];
        p->next = code_type[hashval];                  /* ハッシュ表に値を追加 */
        code_type[hashval] = p;
    }
    return true;
}

/**
 * 命令コードから命令を返す
 * 命令コードでない場合はNULLを返す
 */
CMD *getcmd(WORD code)
{
    CMDTAB *p;
    CMD *c = NULL;

    for(p = code_type[hash_code(code)]; p != NULL; p = p->next) {
        if(code == p->cmd->code) {
            c = p->cmd;
            break;
        }
    }
    return c;
}

/**
 * 命令コードから命令の関数ポインタを返す
 */
const void (*getcmdptr(WORD code))
{
    CMDTAB *t;
    const void (*ptr);

    for(t = code_type[hash_code(code)]; t != NULL; t = t->next) {
        if(code == t->cmd->code) {
            ptr = t->cmd->ptr;
            break;
        }
    }
    return ptr;
}

/**
 * コードがキーの命令ハッシュ表を解放する
 */
void free_code_type()
{
    int i;
    CMDTAB *p, *q;
    for(i = 0; i < CMDTABSIZE; i++) {
        for(p = code_type[i]; p != NULL; p = q) {
            q = p->next;
            FREE(p);
        }
    }
}
