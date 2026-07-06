#include "exec.h"
#include "monitor.h"

/**
 * @brief プログラムレジスタ（PR）を表すWORD値を文字列に変換
 *
 * @return 文字列「PR:XXXX」（Xは16進数の数字）
 *
 * @param pr プログラムレジスタ（PR）を表すWORD値
 */
char *pr2str(WORD pr);

/**
 * @brief 標準入力から文字データを読込（SVC 1）
 *
 */
void svcin();

/**
 * @brief 標準出力へ文字データを書出（SVC 2）
 *
 */
void svcout();

/**
 * @brief ロード／論理積／論理和／排他的論理和のフラグ設定。OFは常に0
 *
 * @param adr アドレス
 */
void setfr(WORD adr);

/**
 * @brief WORD値からr/r1を取得
 *
 * @return r/r1を表すWORD値
 *
 * @param oprx オペランドを表すWORD値
 */
WORD get_r_r1(WORD oprx);

/**
 * @brief WORD値からx/r2を取得
 *
 * @return x/r2を表すWORD値
 *
 * @param oprx オペランドを表すWORD値
 */
WORD get_x_r2(WORD oprx);

/**
 * @brief 2つのWORD値からadr[,x]を取得
 *
 * @return adr[,x]を表すWORD値
 *
 * @param adr アドレスを表すWORD値
 * @param oprx オペランドを表すWORD値
 */
WORD get_adr_x(WORD adr, WORD oprx);

/**
 * @brief 2つのWORD値からadr[,x]のアドレスに格納されている値を取得
 *
 * @return adr[,x]のアドレスに格納されている値を表すWORD値
 *
 * @param adr アドレスを表すWORD値
 * @param oprx オペランドを表すWORD値
 */
WORD get_val_adr_x(WORD adr, WORD oprx);

/**
 * @brief 実行エラーの定義
 */
static CERR cerr_exec[] = {
    { 201, "Program Register (PR) - memory overflow" },
    { 202, "Stack Pointer (SP) - stack overflow" },
    { 203, "Stack Pointer (SP) - stack underflow" },
    { 204, "OP in word #1 - not command code" },
    { 205, "r/r1 in word #1 - not GR" },
    { 206, "x/r2 in word #1 - not GR" },
    { 207, "address in word #2 - out of memory" },
    { 208, "SVC input - memory overflow" },
    { 209, "SVC output - memory overflow" },
    { 214, "CPU Clocks - not positive integer" },
    { 215, "Memory Size - not integer or out of range: 1 - 65536" },
};

/**
 * @brief 実行モード: trace, logical, dump, dump_start, dump_end, reverse, monitor, step
 */
EXECMODE execmode = {false, false, false, 0, 0xFFFF, false, false, false};

CLOCK clock_str2clock(const char *str) {
    long val = str2l_range(str, 1, LONG_MAX, "Clock");
    if(val > MAX_CLOCKS) {
        val = MAX_CLOCKS;
        fprintf(stderr, "Info - %s: Clock frequency exceeds maximum. Set to %ld\n", str, val);
    }
    return (CLOCK)val;
}

char *pr2str(WORD pr)
{
    char *str = malloc_chk(CERRSTRSIZE + 1, "pr2str.pr");

    sprintf(str, "PR:#%04X", pr);
    return str;
}

void svcin()
{
    char *buf = malloc_chk(INSIZE + 1, "svcin.buf");

    if(fgets(buf, INSIZE, stdin) == NULL) {
        sys->memory[sys->cpu->gr[1]] = 0x0;
        sys->memory[sys->cpu->gr[2]] = 0xFFFF;
        FREE(buf);
        return;
    }
    for(int i = 0; i < INSIZE; i++) {
        if(!buf[i] || buf[i] == '\n') {
            break;
        }
        if(sys->cpu->gr[1] + i >= execptr->end) {
            setcerr(208, "");    /* SVC input - memory overflow */
            break;
        }
        sys->memory[sys->cpu->gr[1] + i] = buf[i];
        sys->memory[sys->cpu->gr[2]] = i + 1;
    }
    FREE(buf);
}

void svcout()
{
    int i;
    WORD w;

    for(i = 0; i < sys->memory[sys->cpu->gr[2]]; i++) {
        if(sys->cpu->gr[1] + i >= execptr->end) {
            setcerr(209, "");    /* SVC output - memory overflow */
            return;
        }
        /* 「JIS X 0201ラテン文字・片仮名用8ビット符号で規定する文字の符号表」
           に記載された文字と、改行（CR）／タブを表示 */
        /* それ以外の文字は、「.」で表す */
        if(((w = sys->memory[sys->cpu->gr[1]+i]) >= 0x20 && w <= 0x7E) ||    /* JIS X 0201ラテン文字 */
           (w >= 0xA0 && w <= 0xFE) ||                                       /* JIS X 0201片仮名用8ビット符号 */
           w == 0xA || w == '\t')
        {
            putchar((char)w);
        } else {
            putchar('.');
        }
    }
}

void setfr(WORD adr)
{
    sys->cpu->fr = 0x0;
    /* 第15ビットが1のとき、SFは1 */
    if((adr & 0x8000) == 0x8000) {
        sys->cpu->fr += SF;
    }
    /* 演算結果が0のとき、ZFは1 */
    if(adr == 0x0) {
        sys->cpu->fr += ZF;
    }
}

WORD get_r_r1(WORD oprx)
{
    WORD r = 0;
    char *s = NULL;

    if((r = ((oprx & 0x00F0) >>4)) > GRSIZE - 1) {
        setcerr(205, s = pr2str(sys->cpu->pr));    /* r/r1 in word #1 - not GR */
        FREE(s);
        return 0x0;
    }
    return r;
}

WORD get_x_r2(WORD oprx)
{
    WORD x = 0;
    char *s = NULL;

    if((x = (oprx & 0x000F)) > GRSIZE - 1) {
        setcerr(206, s = pr2str(sys->cpu->pr));    /* r/r1 in word #1 - not GR */
        FREE(s);
        return 0x0;
    }
    return x;
}

WORD get_adr_x(WORD adr, WORD oprx)
{
    WORD a = adr;
    WORD x = get_x_r2(oprx);

    if(x > 0) {
        a += sys->cpu->gr[x];
    }
    return a;
}

WORD get_val_adr_x(WORD adr, WORD oprx)
{
    WORD a = 0;
    char *s = NULL;

    if((a = get_adr_x(adr, oprx)) >= sys->memsize) {
        setcerr(207, s = pr2str(sys->cpu->pr + 1));    /* address in word #2 - out of memory */
        FREE(s);
        return 0x0;
    }
    return sys->memory[a];
}

/* exec.hで定義された関数群 */
void addcerrlist_exec()
{
    addcerrlist(ARRAYSIZE(cerr_exec), cerr_exec);
}

void nop()
{
    sys->cpu->pr += getcmdwordlen("NOP", NONE);
}

void ld_r_adr_x()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    setfr(sys->cpu->gr[get_r_r1(w[0])] = get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += getcmdwordlen("LD", R_ADR_X);
}

void ld_r1_r2()
{
    WORD w[] = {sys->memory[sys->cpu->pr]};
    setfr(sys->cpu->gr[get_r_r1(w[0])] = sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += getcmdwordlen("LD", R1_R2);
}

void st()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    sys->memory[get_adr_x(w[1], w[0])] = sys->cpu->gr[get_r_r1(w[0])];
    sys->cpu->pr += getcmdwordlen("ST", R_ADR_X);
}

void lad()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    sys->cpu->gr[get_r_r1(w[0])] = get_adr_x(w[1], w[0]);
    sys->cpu->pr += getcmdwordlen("LAD", R_ADR_X);
}

void adda(WORD r, WORD val)
{
    long tmp;

    sys->cpu->fr = 0x0;
    /* 引数の値を16ビット符号付整数として加算し、オーバーフローをチェック */
    assert(sizeof(short) * 8 == 16 && (short)0xFFFF == -1);
    if((tmp = (short)(sys->cpu->gr[r]) + (short)val) > 32767 || tmp < -32768) {
        sys->cpu->fr += OF;
    }
    /* 加算した結果を、WORD値に戻す */
    sys->cpu->gr[r] = (WORD)(tmp & 0xFFFF);
    if((sys->cpu->gr[r] & 0x8000) == 0x8000) {
        sys->cpu->fr += SF;
    } else if(sys->cpu->gr[r] == 0x0) {
        sys->cpu->fr += ZF;
    }
}

void adda_r_adr_x()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    adda(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += getcmdwordlen("ADDA", R_ADR_X);
}

void adda_r1_r2()
{
    WORD w[] = {sys->memory[sys->cpu->pr]};
    adda(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += getcmdwordlen("ADDA", R1_R2);
}

void suba_r_adr_x()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    adda(get_r_r1(w[0]), ~(get_val_adr_x(w[1], w[0])) + 1);
    sys->cpu->pr += getcmdwordlen("SUBA", R_ADR_X);
}

void suba_r1_r2()
{
    WORD w[] = {sys->memory[sys->cpu->pr]};
    adda(get_r_r1(w[0]), ~(sys->cpu->gr[get_x_r2(w[0])]) + 1);
    sys->cpu->pr += getcmdwordlen("SUBA", R1_R2);
}

void addl_gr(WORD r, WORD val, bool add)
{
    unsigned long o = 0;
    unsigned long s = 0;

    o = sys->cpu->gr[r];
    sys->cpu->fr = 0;    /* flag initialize */

    if(add == true) {
        s = o + val;
        if(s > 0xFFFF) {
            sys->cpu->fr += OF;
        }
    } else {
        if(o < val) {
            sys->cpu->fr += OF;
        }
        s = o + (~val + 1);
        if(s > 0xFFFF) {
            s &= 0xFFFF;
        }
    }
    sys->cpu->gr[r] = (WORD)s;

    if((s & 0x8000) == 0x8000) {
        sys->cpu->fr += SF;
    }
    else if(s == 0x0) {
        sys->cpu->fr += ZF;
    }
}

void addl_r_adr_x()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    addl_gr(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]), true);
    sys->cpu->pr += getcmdwordlen("ADDL", R_ADR_X);
}

void addl_r1_r2()
{
    WORD w[] = {sys->memory[sys->cpu->pr]};
    addl_gr(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])], true);
    sys->cpu->pr += getcmdwordlen("ADDL", R1_R2);
}

void subl_r_adr_x()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    addl_gr(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]), false);
    sys->cpu->pr += getcmdwordlen("SUBL", R_ADR_X);
}

void subl_r1_r2()
{
    WORD w[] = {sys->memory[sys->cpu->pr]};
    addl_gr(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])], false);
    sys->cpu->pr += getcmdwordlen("SUBL", R1_R2);
}

void and_r_adr_x()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    setfr(sys->cpu->gr[get_r_r1(w[0])] &= get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += getcmdwordlen("AND", R_ADR_X);
}

void and_r1_r2()
{
    WORD w[] = {sys->memory[sys->cpu->pr]};
    setfr(sys->cpu->gr[get_r_r1(w[0])] &= sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += getcmdwordlen("AND", R1_R2);
}

void or_r_adr_x()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    setfr(sys->cpu->gr[get_r_r1(w[0])] |= get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += getcmdwordlen("OR", R_ADR_X);
}

void or_r1_r2()
{
    WORD w[] = {sys->memory[sys->cpu->pr]};
    setfr(sys->cpu->gr[get_r_r1(w[0])] |= sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += getcmdwordlen("OR", R1_R2);
}

void xor_r_adr_x()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    setfr(sys->cpu->gr[get_r_r1(w[0])] ^= get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += getcmdwordlen("XOR", R_ADR_X);
}

void xor_r1_r2()
{
    WORD w[] = {sys->memory[sys->cpu->pr]};
    setfr(sys->cpu->gr[get_r_r1(w[0])] ^= sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += getcmdwordlen("XOR", R1_R2);
}

void cpa(WORD r, WORD val)
{
    sys->cpu->fr = 0;
    if((short)sys->cpu->gr[r] < (short)val) {
        sys->cpu->fr = SF;
    } else if(sys->cpu->gr[r] == val) {
        sys->cpu->fr = ZF;
    }
}

void cpa_r_adr_x()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    cpa(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += getcmdwordlen("CPA", R_ADR_X);
}

void cpa_r1_r2()
{
    WORD w[] = {sys->memory[sys->cpu->pr]};
    cpa(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += getcmdwordlen("CPA", R1_R2);
}

void cpl(WORD r, WORD val)
{
    sys->cpu->fr = 0x0;
    if(sys->cpu->gr[r] < val) {
        sys->cpu->fr = SF;
    } else if(sys->cpu->gr[r] == val) {
        sys->cpu->fr = ZF;
    }
}

void cpl_r_adr_x()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    cpl(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += getcmdwordlen("CPL", R_ADR_X);
}

void cpl_r1_r2()
{
    WORD w[] = {sys->memory[sys->cpu->pr]};
    cpl(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += getcmdwordlen("CPL", R1_R2);
}

void sla()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    WORD r = get_r_r1(w[0]);
    WORD sign = sys->cpu->gr[r] & 0x8000;
    WORD last = 0;

    sys->cpu->fr = 0;
    sys->cpu->gr[r] &= 0x7FFF;
    for(int i = 0; i < get_adr_x(w[1], w[0]); i++) {
        last = sys->cpu->gr[r] & 0x4000;
        sys->cpu->gr[r] <<= 1;
    }
    sys->cpu->gr[r] = sign | (sys->cpu->gr[r] & 0x7FFF);
    /* OFに、レジスタから最後に送り出されたビットの値を設定 */
    if(last > 0x0) {
        sys->cpu->fr += OF;
    }
    /* 符号（第15ビット）が1のとき、SFは1 */
    if(sign > 0x0) {
        sys->cpu->fr += SF;
    }
    /* 演算結果が0のとき、ZFは1 */
    if(sys->cpu->gr[r] == 0x0) {
        sys->cpu->fr += ZF;
    }
    sys->cpu->pr += getcmdwordlen("SLA", R_ADR_X);
}

void sra()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    WORD r = get_r_r1(w[0]);
    WORD sign = sys->cpu->gr[r] & 0x8000;
    WORD last = 0;

    sys->cpu->fr = 0;
    sys->cpu->gr[r] &= 0x7FFF;
    for(int i = 0; i < get_adr_x(w[1], w[0]); i++) {
        last = sys->cpu->gr[r] & 0x1;
        sys->cpu->gr[r] >>= 1;
        if(sign > 0) {
            sys->cpu->gr[r] |= 0x4000;
        }
    }
    sys->cpu->gr[r] = sign | sys->cpu->gr[r];
    /* OFに、レジスタから最後に送り出されたビットの値を設定 */
    if(last > 0x0) {
        sys->cpu->fr += OF;
    }
    /* 符号（第15ビット）が1のとき、SFは1 */
    if(sign > 0x0) {
        sys->cpu->fr += SF;
    }
    /* 演算結果が0のとき、ZFは1 */
    if(sys->cpu->gr[r] == 0x0) {
        sys->cpu->fr += ZF;
    }
    sys->cpu->pr += getcmdwordlen("SRA", R_ADR_X);
}

void sll()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    WORD last = 0;
    WORD r = get_r_r1(w[0]);

    sys->cpu->fr = 0x0;
    for(int i = 0; i < get_adr_x(w[1], w[0]); i++) {
        last = sys->cpu->gr[r] & 0x8000;
        sys->cpu->gr[r] <<= 1;
    }
    /* OFに、レジスタから最後に送り出されたビットの値を設定 */
    if(last > 0x0) {
        sys->cpu->fr += OF;
    }
    /* 第15ビットが1のとき、SFは1 */
    if((sys->cpu->gr[r] & 0x8000) > 0x0) {
        sys->cpu->fr += SF;
    }
    /* 演算結果が0のとき、ZFは1 */
    if(sys->cpu->gr[r] == 0x0) {
        sys->cpu->fr += ZF;
    }
    sys->cpu->pr += getcmdwordlen("SLL", R_ADR_X);
}

void srl()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    WORD last = 0;
    WORD r = get_r_r1(w[0]);

    sys->cpu->fr = 0x0;
    for(int i = 0; i < get_adr_x(w[1], w[0]); i++) {
        last = sys->cpu->gr[r] & 0x0001;
        sys->cpu->gr[r] >>= 1;
    }
    /* OFに、レジスタから最後に送り出されたビットの値を設定 */
    if(last > 0x0) {
        sys->cpu->fr += OF;
    }
    /* 第15ビットが1のとき、SFは1 */
    if((sys->cpu->gr[r] & 0x8000) > 0x0) {
        sys->cpu->fr += SF;
    }
    /* 演算結果が0のとき、ZFは1 */
    if(sys->cpu->gr[r] == 0x0) {
        sys->cpu->fr += ZF;
    }
    sys->cpu->pr += getcmdwordlen("SRL", R_ADR_X);
}

void jmi()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    if((sys->cpu->fr & SF) > 0) {
        sys->cpu->pr = get_adr_x(w[1], w[0]);
    } else {
        sys->cpu->pr += getcmdwordlen("JMI", ADR_X);
    }
}

void jnz()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    if((sys->cpu->fr & ZF) == 0) {
        sys->cpu->pr = get_adr_x(w[1], w[0]);
    } else {
        sys->cpu->pr += getcmdwordlen("JNZ", ADR_X);
    }
}

void jze()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    if((sys->cpu->fr & ZF) > 0) {
        sys->cpu->pr = get_adr_x(w[1], w[0]);
    } else {
        sys->cpu->pr += getcmdwordlen("JZE", ADR_X);
    }
}

void jump()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    sys->cpu->pr = get_adr_x(w[1], w[0]);
}

void jpl()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    if((sys->cpu->fr & (SF | ZF)) == 0) {
        sys->cpu->pr = get_adr_x(w[1], w[0]);
    } else {
        sys->cpu->pr += getcmdwordlen("JPL", ADR_X);
    }
}

void jov()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    if((sys->cpu->fr & OF) > 0) {
        sys->cpu->pr = get_adr_x(w[1], w[0]);
    } else {
        sys->cpu->pr += getcmdwordlen("JOV", ADR_X);
    }
}

void push()
{
    assert(sys->cpu->sp > execptr->end && sys->cpu->sp <= sys->memsize);
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    sys->memory[--(sys->cpu->sp)] = get_adr_x(w[1], w[0]);
    sys->cpu->pr += getcmdwordlen("PUSH", ADR_X);
}

void pop()
{
    assert(sys->cpu->sp > execptr->end);
    WORD w[] = {sys->memory[sys->cpu->pr]};
    char *s = NULL;

    if(sys->cpu->sp >= sys->memsize) {
        setcerr(203, s = pr2str(sys->cpu->pr));        /* Stack Pointer (SP) - stack underflow */
        FREE(s);
    } else {
        sys->cpu->gr[get_r_r1(w[0])] = sys->memory[(sys->cpu->sp)++];
        sys->cpu->pr += getcmdwordlen("POP", R_);
    }
}

void call()
{
    assert(sys->cpu->sp > execptr->end && sys->cpu->sp <= sys->memsize);
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    sys->memory[--(sys->cpu->sp)] = sys->cpu->pr + 1;
    sys->cpu->pr = get_adr_x(w[1], w[0]);
}

void ret()
{
    assert(sys->cpu->sp <= sys->memsize);
    if(sys->cpu->sp == sys->memsize) {
        execptr->stop = true;
    } else if(sys->cpu->sp < sys->memsize) {
        sys->cpu->pr = sys->memory[(sys->cpu->sp)++] + 1;
    }
}

void svc()
{
    WORD w[] = {sys->memory[sys->cpu->pr], sys->memory[sys->cpu->pr + 1]};
    switch(get_adr_x(w[1], w[0]))
    {
    case 0x0:                   /* STOP */
        execptr->stop = true;
        break;
    case 0x1:                   /* IN */
        svcin();
        break;
    case 0x2:                   /* OUT */
        svcout();
        break;
    }
    sys->cpu->pr += getcmdwordlen("SVC", ADR_X);
}

void exec()
{
    CLOCK clock_begin = 0;
    CLOCK clock_end = 0;
    void (*cmdptr)() = NULL;
    char *s = NULL;
    const char *monmsg = "COMET II machine code monitor. Type ? for help.\n";

    create_cmdtable(HASH_CODE);                 /* 命令コードがキーのハッシュ表を作成 */

    /* 機械語の実行 */
    sys->cpu->pr = execptr->start;
    do {
        clock_begin = clock();                     /* クロック周波数設定のため、実行開始時間を格納 */
        dsp_trace_dump();
        /* モニターモードでCOMET II起動時、またはステップモードかブレークポイントの場合、モニターを起動 */
        if((execmode.monitor == true && execptr->started == false) || execptr->stop == true || execmode.step == true || getbps(sys->cpu->pr) == true)
        {
            /* モニターモードでCOMET II起動時、フラグを立て、メッセージを表示 */
            if(execptr->started == false) {
                execptr->started = true;
                fprintf(stdout, "%s", monmsg);
            }
            monitor();
            if(execmode.monitor == false && (execptr->start == false || execptr->stop == true)) {
                break;
            }
        }
        /* プログラムレジスタをチェック */
        if(sys->cpu->pr >= sys->memsize) {
            setcerr(201, s = pr2str(sys->cpu->pr));        /* Program Register (PR) - memory overflow */
            goto execfin;
        }
        /* スタックポインタをチェック */
        if(sys->cpu->sp <= execptr->end) {
            setcerr(202, s = pr2str(sys->cpu->pr));        /* Stack Pointer (SP) - stack overflow */
            goto execfin;
        }
        /* コードから命令を取得 */
        /* 取得できない場合はエラー終了 */
        if((cmdptr = getcmdptr(sys->memory[sys->cpu->pr] & 0xFF00)) == NULL) {
            setcerr(204, s = pr2str(sys->cpu->pr));            /* OP in word #1 - not command code */
            goto execfin;
        }
        /* 命令の実行 */
        (*cmdptr)();
        /* エラー発生時はエラー終了 */
        if(cerr->num > 0) {
            goto execfin;
        }
        /* 終了フラグがtrueの場合は、モニターまたは正常終了 */
        if(execptr->stop == true) {
            if(execmode.monitor == true) {
                fprintf(stdout, "Return to top.\n");
            } else {
                break;
            }
        }
        /* クロック周波数の設定 */
        do {
            clock_end = clock();
        } while(clock_end - clock_begin < CLOCKS_PER_SEC / sys->clocks);
    } while(1);
execfin:
    FREE(s);
    freebps();
    free_cmdtable(HASH_CODE);              /* 命令のコードとタイプがキーのハッシュ表を解放 */
    if(cerr->num > 0) {
        fprintf(stderr, "Execute error - %d: %s\n", cerr->num, cerr->msg);
    }
}
