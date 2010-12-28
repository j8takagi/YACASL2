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
 * 命令表のサイズ
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
    free_chk(keys[0]->val.s, "keys[0].val.s");
    free_chk(keys[0], "keys[0]");
    free_chk(keys[1], "keys[1]");
    /* ハッシュ値を返す */
    return hashval;
}

/**
 * 名前とタイプがキーの命令ハッシュ表を作成する
 */
bool create_cmdtype_code()
{
    CMDTAB *np;
    unsigned hashval;
    int i;

    cmdtabsize = comet2cmdsize;
    cmdtype_code = calloc_chk(cmdtabsize, sizeof(CMDTAB *), "cmdtype_code");
    for(i = 0; i < cmdtabsize; i++) {
        *(cmdtype_code + i) = NULL;
    }
    for(i = 0; i < comet2cmdsize; i++) {
        np = malloc_chk(sizeof(CMDTAB), "cmdtype_code.next");
        np->cmd = NULL;
        np->next = NULL;
        /* ハッシュ値の生成 */
        hashval = hash_cmdtype(comet2cmd[i].name, comet2cmd[i].type);
        /* ハッシュ表に値を追加 */
        np->next = cmdtype_code[hashval];
        cmdtype_code[hashval] = np;
        np->cmd = &(comet2cmd[i]);
    }
    return true;
}

/**
 * 命令の名前とタイプから、命令コードを返す
 * 無効な場合は0xFFFFを返す
 */
WORD getcmdcode(const char *cmd, CMDTYPE type)
{
    CMDTAB *np;

    assert(cmd != NULL);
    for(np = cmdtype_code[hash_cmdtype(cmd, type)]; np != NULL; np = np->next){
        if(strcmp(cmd, np->cmd->name) == 0 && type == np->cmd->type) {
            return np->cmd->code;
        }
    }
    return 0xFFFF;
}

/**
 * 名前とタイプがキーの命令ハッシュ表を解放する
 */
void free_cmdtype_code()
{
    int i;
    CMDTAB *np, *nq;

    for(i = 0; i < cmdtabsize; i++) {
        np = cmdtype_code[i];
        while(np != NULL) {
            nq = np->next;
            free_chk(np, "free_cmdtype_code.np");
            np = nq;
        }
    }
    free_chk(cmdtype_code, "cmdtype_code");
}

/**
 * 命令コードからハッシュ値を生成する
 */
unsigned hash_code(WORD code)
{
    HKEY *keys[1];

    /* 命令コードを設定 */
    keys[0] = malloc_chk(sizeof(HKEY), "hash_code.key");
    keys[0]->type = INT;
    keys[0]->val.i = (int)(code >> 8);
    /* ハッシュ値を返す */
    return hash(1, keys, cmdtabsize);
}

/**
 * コードがキーの命令ハッシュ表を作成する
 */
bool create_code_type()
{
    CMDTAB *np;
    unsigned hashval;
    int i;

    cmdtabsize = comet2cmdsize;
    code_type = calloc_chk(cmdtabsize, sizeof(CMDTAB *), "code_type");
    for(i = 0; i < cmdtabsize; i++) {
        *(code_type + i) = NULL;
    }
    for(i = 0; i < comet2cmdsize; i++) {
        np = malloc_chk(sizeof(CMDTAB), "code_type.next");
        np->cmd = NULL;
        np->next = NULL;
        /* ハッシュ値の生成 */
        hashval = hash_code((&comet2cmd[i])->code);
        /* ハッシュ表に値を追加 */
        np->next = code_type[hashval];
        code_type[hashval] = np;
        np->cmd = &comet2cmd[i];
    }
    return true;
}

/**
 * 命令コードから命令タイプを返す
 * 無効な場合はNONEを返す
 */
CMDTYPE getcmdtype(WORD code)
{
    CMDTAB *np;
    for(np = code_type[hash_code(code)]; np != NULL; np = np->next) {
        if(code == np->cmd->code) {
            return np->cmd->type;
        }
    }
    return NONE;
}

/**
 * コードがキーの命令ハッシュ表を解放する
 */
void free_code_type()
{
    int i;
    CMDTAB *np, *nq;
    for(i = 0; i < cmdtabsize; i++) {
        np = code_type[i];
        while(np != NULL) {
            nq = np->next;
            free_chk(np, "np");
            np = nq;
        }
    }
    free_chk(code_type, "code_type");
}
