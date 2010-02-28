#include "casl2.h"

CMDCODEARRAY cmdcodearray[] = {
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

int cmdcodesize = ARRAYSIZE(cmdcodearray);
int cmdtabsize;
CMDCODETAB **cmdtype_code, **code_type;

/* 命令と命令タイプからハッシュ値を生成する */
unsigned hash_cmdtype(const char *cmd, CMDTYPE type) {
    HKEY *keys[2];

    /* 命令をセット */
    keys[0] = malloc(sizeof(HKEY));
    keys[0]->type = CHARS;
    keys[0]->val.s = strdup(cmd);
    /* 命令タイプをセット */
    keys[1] = malloc(sizeof(HKEY));
    keys[1]->type = INT;
    keys[1]->val.i = (int)(type & 070);
    /* ハッシュ値を返す */
    return hash(2, keys, cmdtabsize);
}

/* 命令と命令タイプがキーのハッシュ表を作成する */
bool create_cmdtype_code()
{
    CMDCODETAB *np;
    unsigned hashval;
    int i;

    cmdtabsize = cmdcodesize;
    cmdtype_code = malloc(cmdtabsize * sizeof(CMDCODETAB *));
    for(i = 0; i < cmdcodesize; i++) {
        if((np = malloc(sizeof(CMDCODETAB))) == NULL) {
            setcerr(122, NULL);    /* cannot create hash table */
            return false;
        }
        /* ハッシュ値の生成 */
        hashval = hash_cmdtype(cmdcodearray[i].cmd, cmdcodearray[i].type);
        /* ハッシュ表に値を追加 */
        np->next = cmdtype_code[hashval];
        cmdtype_code[hashval] = np;
        np->cca = &(cmdcodearray[i]);
    }
    return true;
}

/* 命令と命令タイプから、命令コードを取得する */
/* 無効な場合は0xFFFFを返す */
WORD getcmdcode(const char *cmd, CMDTYPE type)
{
    CMDCODETAB *np;
    assert(cmd != NULL);
    for(np = cmdtype_code[hash_cmdtype(cmd, type)]; np != NULL; np = np->next){
        if(strcmp(cmd, np->cca->cmd) == 0 && type == np->cca->type) {
            return np->cca->code;
        }
    }
    return 0xFFFF;
}

/* 命令と命令タイプがキーのハッシュ表を解放する */
void free_cmdtype_code()
{
    int i;
    CMDCODETAB *np, *nq;
    for(i = 0; i < cmdtabsize; i++){
        np = cmdtype_code[i];
        while(np != NULL) {
            nq = np->next;
            free(np);
            np = nq;
        }
    }
}

/* 命令コードからハッシュ値を生成する */
unsigned hash_code(WORD code)
{
    HKEY *keys[1];

    /* 命令コードをセット */
    keys[0] = malloc(sizeof(HKEY));
    keys[0]->type = INT;
    keys[0]->val.i = (int)(code >> 8);
    /* ハッシュ値を返す */
    return hash(1, keys, cmdtabsize);
}

/* 命令コードがキーのハッシュ表を作成する */
bool create_code_type()
{
    CMDCODETAB *np;
    unsigned hashval;
    int i;

    cmdtabsize = hashtabsize(cmdcodesize);
    code_type = malloc(cmdtabsize * sizeof(CMDCODETAB *));
    for(i = 0; i < cmdcodesize; i++) {
        if((np = malloc(sizeof(CMDCODETAB))) == NULL) {
            setcerr(122, NULL);    /* cannot create hash table */
            return false;
        }
        /* ハッシュ値の生成 */
        hashval = hash_code((&cmdcodearray[i])->code);
        /* ハッシュ表に値を追加 */
        np->next = code_type[hashval];
        code_type[hashval] = np;
        np->cca = &cmdcodearray[i];
    }
    return true;
}

/* 命令コードから命令タイプを取得する */
/* 無効な場合はNONEを返す */
CMDTYPE getcmdtype(WORD code)
{
    CMDCODETAB *np;
    for(np = code_type[hash_code(code)]; np != NULL; np = np->next) {
        if(code == np->cca->code) {
            return np->cca->type;
        }
    }
    return NONE;
}

/* 命令コードがキーのハッシュ表を解放する */
void free_code_type()
{
    int i;
    CMDCODETAB *np, *nq;
    for(i = 0; i < cmdtabsize; i++){
        np = code_type[i];
        while(np != NULL) {
            nq = np->next;
            free(np);
            np = nq;
        }
    }
}
