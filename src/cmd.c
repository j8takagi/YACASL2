#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "hash.h"
#include "struct.h"
#include "cmem.h"

/**
 * 機械語命令のリスト
 */
static CMD comet2cmd[] = {
    { "NOP", NONE, 0x0 },
    { "LD", R_ADR_X_, 0x1000 },
    { "ST", R_ADR_X, 0x1100 },
    { "LAD", R_ADR_X, 0x1200 },
    { "LD", R1_R2, 0x1400 },
    { "ADDA", R_ADR_X_, 0x2000 },
    { "SUBA", R_ADR_X_, 0x2100 },
    { "ADDL", R_ADR_X_, 0x2200 },
    { "SUBL", R_ADR_X_, 0x2300 },
    { "ADDA", R1_R2, 0x2400 },
    { "SUBA", R1_R2, 0x2500 },
    { "ADDL", R1_R2, 0x2600 },
    { "SUBL", R1_R2, 0x2700 },
    { "AND", R_ADR_X_, 0x3000 },
    { "OR", R_ADR_X_, 0x3100 },
    { "XOR", R_ADR_X_, 0x3200 },
    { "AND", R1_R2, 0x3400 },
    { "OR", R1_R2, 0x3500 },
    { "XOR", R1_R2, 0x3600 },
    { "CPA", R_ADR_X_, 0x4000 },
    { "CPL", R_ADR_X_, 0x4100 },
    { "CPA", R1_R2, 0x4400 },
    { "CPL", R1_R2, 0x4500 },
    { "SLA", R_ADR_X, 0x5000 },
    { "SRA", R_ADR_X, 0x5100 },
    { "SLL", R_ADR_X, 0x5200 },
    { "SRL", R_ADR_X, 0x5300 },
    { "JMI", ADR_X, 0x6100 },
    { "JNZ", ADR_X, 0x6200 },
    { "JZE", ADR_X, 0x6300 },
    { "JUMP", ADR_X, 0x6400 },
    { "JPL", ADR_X, 0x6500 },
    { "JOV", ADR_X, 0x6600 },
    { "PUSH", ADR_X, 0x7000 },
    { "POP", R_, 0x7100 },
    { "CALL", ADR_X, 0x8000 },
    { "SVC", ADR_X, 0xF000 },
    { "RET", NONE, 0x8100 },
};

/**
 * 命令コード配列のサイズ
 */
static int comet2cmdsize = ARRAYSIZE(comet2cmd);

/**
 * ハッシュ表のサイズ
 */
static int cmdtabsize;

/**
 * ハッシュ表
 */
static CMDTAB **cmdtype_code, **code_type;

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
    hashval = hash(2, keys, cmdtabsize);
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

    cmdtabsize = comet2cmdsize;                                            /* ハッシュ表のサイズ */
    cmdtype_code = calloc_chk(cmdtabsize, sizeof(CMDTAB **), "cmdtype_code");
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

    for(i = 0; i < cmdtabsize; i++) {
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
    h = hash(1, keys, cmdtabsize);
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

    cmdtabsize = comet2cmdsize;                    /* ハッシュ表のサイズ */
    code_type = calloc_chk(comet2cmdsize, sizeof(CMDTAB **), "code_type");
    for(i = 0; i < comet2cmdsize; i++) {
        hashval = hash_code((&comet2cmd[i])->code);    /* ハッシュ値の生成 */
        p = malloc_chk(sizeof(CMDTAB **), "code_type");
        p->cmd = &comet2cmd[i];
        p->next = code_type[hashval];                  /* ハッシュ表に値を追加 */
        code_type[hashval] = p;
    }
    return true;
}

/**
 * 命令コードから命令タイプを返す
 * 無効な場合はNOTCMDを返す
 */
CMDTYPE getcmdtype(WORD code)
{
    CMDTAB *p;
    CMDTYPE t = NOTCMD;

    for(p = code_type[hash_code(code)]; p != NULL; p = p->next) {
        if(code == p->cmd->code) {
            t = p->cmd->type;
            break;
        }
    }
    return t;
}

/**
 * コードがキーの命令ハッシュ表を解放する
 */
void free_code_type()
{
    int i;
    CMDTAB *p, *q;
    for(i = 0; i < cmdtabsize; i++) {
        for(p = code_type[i]; p != NULL; p = q) {
            q = p->next;
            FREE(p);
        }
    }
}
