#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "exec.h"
#include "cerr.h"

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
 * @return なし
 */
void svcin();

/**
 * @brief 標準出力へ文字データを書出（SVC 2）
 *
 * @return なし
 */
void svcout();

/**
 * @brief ロード／論理積／論理和／排他的論理和のフラグ設定。OFは常に0
 *
 * @return なし
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
};

/**
 * @brief アセンブル結果読み込みエラーの定義
 */
static CERR cerr_load[] = {
    { 210, "load - memory overflow" },
    { 211, "object file not specified" },
};

/**
 * @brief 実行モード: trace, logical, dump
 */
EXECMODE execmode = {false, false, false};

char *pr2str(WORD pr)
{
    char *str = malloc_chk(CERRSTRSIZE + 1, "pr2str.pr");

    sprintf(str, "PR:#%04X", pr);
    return str;
}

void svcin()
{
    int i;
    char *buffer = malloc_chk(INSIZE + 1, "svcin.buffer");

    if(fgets(buffer, INSIZE, stdin) == NULL) {
        sys->memory[sys->cpu->gr[1]] = sys->memory[sys->cpu->gr[2]] = 0x0;
        return;
    }
    for(i = 0; i < INSIZE; i++) {
        if(*(buffer + i) == '\0' || *(buffer + i) == '\n') {
            --i;
            break;
        }
        if(sys->cpu->gr[1] + i > execptr->end) {
            setcerr(208, "");    /* SVC input - memory overflow */
            break;
        }
        sys->memory[sys->cpu->gr[1]+i] = *(buffer + i);
    }
    sys->memory[sys->cpu->gr[2]] = i + 1;
    FREE(buffer);
}

void svcout()
{
    int i;
    WORD w;

    for(i = 0; i < sys->memory[sys->cpu->gr[2]]; i++) {
        if(sys->cpu->gr[1] + i > execptr->end) {
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
    WORD r;
    char *s;

    if((r = ((oprx & 0x00F0) >>4)) > GRSIZE - 1) {
        setcerr(205, s = pr2str(sys->cpu->pr));    /* r/r1 in word #1 - not GR */
        FREE(s);
        return 0x0;
    }
    return r;
}

WORD get_x_r2(WORD oprx)
{
    WORD x;
    char *s;

    if((x = (oprx & 0x000F)) > GRSIZE - 1) {
        setcerr(206, s = pr2str(sys->cpu->pr));    /* r/r1 in word #1 - not GR */
        FREE(s);
        return 0x0;
    }
    return x;
}

WORD get_adr_x(WORD adr, WORD oprx)
{
    WORD a = adr, x;
    if((x = get_x_r2(oprx)) > 0) {
        a += sys->cpu->gr[x];
    }
    return a;
}

WORD get_val_adr_x(WORD adr, WORD oprx)
{
    WORD a;
    char *s;

    if((a = get_adr_x(adr, oprx)) >= sys->memsize) {
        setcerr(207, s = pr2str(sys->cpu->pr + 1));    /* address in word #2 - out of memory */
        FREE(s);
        return 0x0;
    }
    return sys->memory[a];
}

/* exec.hで定義された関数群 */
void addcerrlist_load()
{
    addcerrlist(ARRAYSIZE(cerr_load), cerr_load);
}

void addcerrlist_exec()
{
    addcerrlist(ARRAYSIZE(cerr_exec), cerr_exec);
}

bool loadassemble(const char *file)
{
    FILE *fp;
    bool stat = true;

    assert(file != NULL);
    if((fp = fopen(file, "rb")) == NULL) {
        perror(file);
        return false;
    }
    execptr->end = execptr->start +
        fread(sys->memory, sizeof(WORD), sys->memsize - execptr->start, fp);
    if(execptr->end == sys->memsize) {
        setcerr(210, file);    /* load - memory overflow */
        fprintf(stderr, "Load error - %d: %s\n", cerr->num, cerr->msg);
        stat = false;
    }
    fclose(fp);
    return stat;
}

void nop()
{
    sys->cpu->pr += 1;
}

void ld_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    setfr(sys->cpu->gr[get_r_r1(w[0])] = get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

void ld_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    setfr(sys->cpu->gr[get_r_r1(w[0])] = sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

void st()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->memory[get_adr_x(w[1], w[0])] = sys->cpu->gr[get_r_r1(w[0])];
    sys->cpu->pr += 2;
}

void lad()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->cpu->gr[get_r_r1(w[0])] = get_adr_x(w[1], w[0]);
    sys->cpu->pr += 2;
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
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    adda(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

void adda_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    adda(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

void suba_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    adda(get_r_r1(w[0]), ~(get_val_adr_x(w[1], w[0])) + 1);
    sys->cpu->pr += 2;
}

void suba_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    adda(get_r_r1(w[0]), ~(sys->cpu->gr[get_x_r2(w[0])]) + 1);
    sys->cpu->pr += 1;
}

void addl_gr(WORD r, WORD val, bool add)
{
    unsigned long o, s;

    o = sys->cpu->gr[r];
    sys->cpu->fr = 0x0;    /* flag initialize */

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
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    addl_gr(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]), true);
    sys->cpu->pr += 2;
}

void addl_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    addl_gr(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])], true);
    sys->cpu->pr += 1;
}

void subl_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    addl_gr(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]), false);
    sys->cpu->pr += 2;
}

void subl_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    addl_gr(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])], false);
    sys->cpu->pr += 1;
}

void and_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    setfr(sys->cpu->gr[get_r_r1(w[0])] &= get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

void and_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    setfr(sys->cpu->gr[get_r_r1(w[0])] &= sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

void or_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    setfr(sys->cpu->gr[get_r_r1(w[0])] |= get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

void or_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    setfr(sys->cpu->gr[get_r_r1(w[0])] |= sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

void xor_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    setfr(sys->cpu->gr[get_r_r1(w[0])] ^= get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

void xor_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    setfr(sys->cpu->gr[get_r_r1(w[0])] ^= sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

void cpa(WORD r, WORD val)
{
    sys->cpu->fr = 0x0;
    if((short)sys->cpu->gr[r] < (short)val) {
        sys->cpu->fr = SF;
    } else if(sys->cpu->gr[r] == val) {
        sys->cpu->fr = ZF;
    }
}

void cpa_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    cpa(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

void cpa_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    cpa(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
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
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    cpl(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

void cpl_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    cpl(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

void sla()
{
    WORD w[2], sign, last = 0x0, r;
    int i;

    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->cpu->fr = 0x0;
    sign = sys->cpu->gr[(r = get_r_r1(w[0]))] & 0x8000;
    sys->cpu->gr[r] &= 0x7FFF;
    for(i = 0; i < get_adr_x(w[1], w[0]); i++) {
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
    sys->cpu->pr += 2;
}

void sra()
{
    WORD w[2], sign, last = 0x0, r;
    int i;

    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->cpu->fr = 0x0;
    sign = sys->cpu->gr[(r = get_r_r1(w[0]))] & 0x8000;
    sys->cpu->gr[r] &= 0x7FFF;
    for(i = 0; i < get_adr_x(w[1], w[0]); i++) {
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
    sys->cpu->pr += 2;
}

void sll()
{
    WORD w[2], last = 0x0, r;
    int i;

    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->cpu->fr = 0x0;
    for(i = 0; i < get_adr_x(w[1], w[0]); i++) {
        last = sys->cpu->gr[(r = get_r_r1(w[0]))] & 0x8000;
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
    sys->cpu->pr += 2;
}

void srl()
{
    WORD w[2], last = 0x0, r;
    int i;

    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->cpu->fr = 0x0;
    r = get_r_r1(w[0]);
    for(i = 0; i < get_adr_x(w[1], w[0]); i++) {
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
    sys->cpu->pr += 2;
}

void jpl()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    if((sys->cpu->fr & (SF | ZF)) == 0) {
        sys->cpu->pr = get_adr_x(w[1], w[0]);
    } else {
        sys->cpu->pr += 2;
    }
}

void jmi()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    if((sys->cpu->fr & SF) > 0) {
        sys->cpu->pr = get_adr_x(w[1], w[0]);
    } else {
        sys->cpu->pr += 2;
    }
}

void jnz()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    if((sys->cpu->fr & ZF) == 0) {
        sys->cpu->pr = get_adr_x(w[1], w[0]);
    } else {
        sys->cpu->pr += 2;
    }
}

void jze()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    if((sys->cpu->fr & ZF) > 0) {
        sys->cpu->pr = get_adr_x(w[1], w[0]);
    } else {
        sys->cpu->pr += 2;
    }
}

void jov()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    if((sys->cpu->fr & OF) > 0) {
        sys->cpu->pr = get_adr_x(w[1], w[0]);
    } else {
        sys->cpu->pr += 2;
    }
}

void jump()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->cpu->pr = get_adr_x(w[1], w[0]);
}

void push()
{
    assert(sys->cpu->sp > execptr->end && sys->cpu->sp <= sys->memsize);
    WORD w[2];

    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->memory[--(sys->cpu->sp)] = get_adr_x(w[1], w[0]);
    sys->cpu->pr += 2;
}

void pop()
{
    assert(sys->cpu->sp > execptr->end);
    WORD w;
    char *s;

    if(sys->cpu->sp >= sys->memsize) {
        setcerr(203, s = pr2str(sys->cpu->pr));        /* Stack Pointer (SP) - stack underflow */
        FREE(s);
    } else {
        w = sys->memory[sys->cpu->pr];
        sys->cpu->gr[get_r_r1(w)] = sys->memory[(sys->cpu->sp)++];
        sys->cpu->pr += 1;
    }
}

void call()
{
    assert(sys->cpu->sp > execptr->end && sys->cpu->sp <= sys->memsize);
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
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
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
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
    sys->cpu->pr += 2;
}

void exec()
{
    clock_t clock_begin, clock_end;
    void (*cmdptr)();
    char *s;

    create_code_cmdtype();                          /* 命令のコードとタイプがキーのハッシュ表を作成 */
    if(execmode.trace == true) {
        fprintf(stdout, "\nExecuting machine codes\n");
    }
    /* 機械語の実行 */
    for (sys->cpu->pr = execptr->start; ; ) {
        clock_begin = clock();                     /* クロック周波数設定のため、実行開始時間を格納 */
        if(execmode.dump || execmode.trace) {      /* traceまたはdumpオプション指定時、改行を出力 */
            if(execmode.trace) {                   /* traceオプション指定時、レジスタを出力 */
                fprintf(stdout, "#%04X: Register::::\n", sys->cpu->pr);
                dspregister();
            }
            if(execmode.dump) {                    /* dumpオプション指定時、メモリを出力 */
                fprintf(stdout, "#%04X: Memory::::\n", sys->cpu->pr);
                dumpmemory();
            }
            fprintf(stdout, "\n");
        }
        /* プログラムレジスタをチェック */
        if(sys->cpu->pr >= sys->memsize) {
            setcerr(201, s = pr2str(sys->cpu->pr));        /* Program Register (PR) - memory overflow */
            FREE(s);
            goto execfin;
        }
        /* スタックポインタをチェック */
        if(sys->cpu->sp <= execptr->end) {
            setcerr(202, s = pr2str(sys->cpu->pr));        /* Stack Pointer (SP) - stack overflow */
            FREE(s);
            goto execfin;
        }
        /* コードから命令を取得 */
        /* 取得できない場合はエラー終了 */
        if((cmdptr = getcmdptr(sys->memory[sys->cpu->pr] & 0xFF00)) == NULL) {
            setcerr(204, s = pr2str(sys->cpu->pr));            /* OP in word #1 - not command code */
            FREE(s);
            goto execfin;
        }
        /* 命令の実行 */
        (*cmdptr)();
        /* エラー発生時はエラー終了 */
        if(cerr->num > 0) {
            goto execfin;
        }
        /* 終了フラグがtrueの場合は、正常終了 */
        if(execptr->stop == true) {
            break;
        }
        /* クロック周波数の設定 */
        do {
            clock_end = clock();
        } while(clock_end - clock_begin < CLOCKS_PER_SEC / sys->clocks);
    }
execfin:
    free_code_cmdtype();                           /* 命令のコードとタイプがキーのハッシュ表を解放 */
    if(cerr->num > 0) {
        fprintf(stderr, "Execute error - %d: %s\n", cerr->num, cerr->msg);
    }
}
