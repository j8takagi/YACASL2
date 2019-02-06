#include "struct.h"
#include "exec.h"

/**
 * COMET IIの仮想実行マシンシステム
 */
SYSTEM *sys;

/**
 * プログラム実行時の開始と終了のアドレス
 */
EXECPTR *execptr;

/**
 * システムCOMET IIの命令表
 */
static const COMET2CMD comet2cmd[] = {
    { "NOP", NONE, 0x0, nop },
    { "LD", R_ADR_X, 0x1000, ld_r_adr_x },
    { "ST", R_ADR_X, 0x1100, st },
    { "LAD", R_ADR_X, 0x1200, lad },
    { "LD", R1_R2, 0x1400, ld_r1_r2 },
    { "ADDA", R_ADR_X, 0x2000, adda_r_adr_x },
    { "SUBA", R_ADR_X, 0x2100, suba_r_adr_x },
    { "ADDL", R_ADR_X, 0x2200, addl_r_adr_x },
    { "SUBL", R_ADR_X, 0x2300, subl_r_adr_x },
    { "ADDA", R1_R2, 0x2400, adda_r1_r2 },
    { "SUBA", R1_R2, 0x2500, suba_r1_r2 },
    { "ADDL", R1_R2, 0x2600, addl_r1_r2 },
    { "SUBL", R1_R2, 0x2700, subl_r1_r2 },
    { "AND", R_ADR_X, 0x3000, and_r_adr_x },
    { "OR", R_ADR_X, 0x3100, or_r_adr_x },
    { "XOR", R_ADR_X, 0x3200, xor_r_adr_x },
    { "AND", R1_R2, 0x3400, and_r1_r2 },
    { "OR", R1_R2, 0x3500, or_r1_r2 },
    { "XOR", R1_R2, 0x3600, xor_r1_r2 },
    { "CPA", R_ADR_X, 0x4000, cpa_r_adr_x },
    { "CPL", R_ADR_X, 0x4100, cpl_r_adr_x },
    { "CPA", R1_R2, 0x4400, cpa_r1_r2 },
    { "CPL", R1_R2, 0x4500, cpl_r1_r2 },
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
 * 命令表のサイズ
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
static CMDTAB *cmdtab[HASH_MAX][CMDTABSIZE];

/**
 * 命令の名前とタイプからハッシュ値を生成する
 */
unsigned hash_cmdtype(const char *cmd, CMDTYPE type);

/**
 * 命令コードからハッシュ値を生成する
 */
unsigned hash_code(WORD code);

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
 * 命令ハッシュ表を作成する
 */
bool create_cmdtable(CMDTAB_HASH hash)
{
    CMDTAB *p;
    unsigned hashval;
    int i;

    for(i = 0; i < comet2cmdsize; i++) {
        p = malloc_chk(sizeof(CMDTAB), "create_cmdtable.p");
        p->cmd = &comet2cmd[i];
        if(hash == HASH_CMDTYPE) {
            hashval = hash_cmdtype(comet2cmd[i].name, comet2cmd[i].type);
        } else if(hash == HASH_CODE) {
            hashval = hash_code(comet2cmd[i].code);
        }
        p->next = cmdtab[hash][hashval];
        cmdtab[hash][hashval] = p;
    }
    return true;
}

/**
 * 命令ハッシュ表を解放する
 */
void free_cmdtable(CMDTAB_HASH hash)
{
    int i;
    CMDTAB *p, *q;

    for(i = 0; i < CMDTABSIZE; i++) {
        for(p = cmdtab[hash][i]; p != NULL; p = q) {
            q = p->next;
            FREE(p);
        }
        cmdtab[hash][i] = NULL;
    }
}

/**
 * 命令の名前とタイプから、命令コードを返す\n
 * 無効な場合は0xFFFFを返す
 */
WORD getcmdcode(const char *cmd, CMDTYPE type)
{
    CMDTAB *p;
    WORD w = 0xFFFF;

    assert(cmd != NULL);
    for(p = cmdtab[HASH_CMDTYPE][hash_cmdtype(cmd, type)]; p != NULL; p = p->next) {
        if(strcmp(cmd, p->cmd->name) == 0 && type == p->cmd->type) {
            w = p->cmd->code;
            break;
        }
    }
    return w;
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
 * 命令コードから命令の関数ポインタを返す
 */
const void (*getcmdptr(WORD code))
{
    CMDTAB *t;
    const void *ptr = NULL;

    for(t = cmdtab[HASH_CODE][hash_code(code)]; t != NULL; t = t->next) {
        if(code == t->cmd->code) {
            ptr = t->cmd->ptr;
            break;
        }
    }
    return ptr;
}

/**
 * 命令コードから命令のタイプを返す
 */
CMDTYPE getcmdtype(WORD code)
{
    CMDTAB *t;
    CMDTYPE type = NONE;

    for(t = cmdtab[HASH_CODE][hash_code(code)]; t != NULL; t = t->next) {
        if(code == t->cmd->code) {
            type = t->cmd->type;
            break;
        }
    }
    return type;
}

/**
 * 命令コードから命令の名前を返す
 */
char *getcmdname(WORD code)
{
    CMDTAB *t;
    char *cmd = NULL;

    for(t = cmdtab[HASH_CODE][hash_code(code)]; t != NULL; t = t->next) {
        if(code == t->cmd->code) {
            cmd = t->cmd->name;
            break;
        }
    }
    return cmd;
}

/**
 * 汎用レジスタの番号からレジスタを表す文字列を返す
 */

char *grstr(WORD word)
{
    assert(word <= 7);
    char *str = malloc_chk(3 + 1, "grstr.str");
    sprintf(str, "GR%d", word);
    return str;
}

/**
 * COMET II仮想マシンのリセット
 */
void reset(int memsize, int clocks)
{
    int i;

    sys = malloc_chk(sizeof(SYSTEM), "sys");
    /* メモリサイズを設定 */
    sys->memsize = memsize;
    /* クロック周波数を設定 */
    sys->clocks = clocks;
    /* メモリを初期化 */
    sys->memory = calloc_chk(sys->memsize, sizeof(WORD), "memory");
    /* CPUを初期化 */
    sys->cpu = malloc_chk(sizeof(CPU), "cpu");
    for(i = 0; i < GRSIZE; i++) {                    /* 汎用レジスタ  */
        sys->cpu->gr[i] = 0x0;
    }
    sys->cpu->sp = sys->memsize;   /* スタックポインタ */
    sys->cpu->pr = 0x0;            /* プログラムレジスタ */
    sys->cpu->fr = 0x0;            /* フラグレジスタ */
    /* CASL2プログラムの開始と終了のアドレスを初期化 */
    execptr = malloc_chk(sizeof(EXECPTR), "execptr");
    execptr->stop = false;
}

/**
 * COMET II仮想マシンのシャットダウン
 */
void shutdown()
{
    FREE(execptr);
    FREE(sys->memory);
    FREE(sys->cpu);
    FREE(sys);
}
