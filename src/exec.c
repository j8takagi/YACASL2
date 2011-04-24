#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "exec.h"
#include "cerr.h"

/**
 * 実行エラーの定義
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
 * アセンブル結果読み込みエラーの定義
 */
static CERR cerr_load[] = {
    { 210, "load - memory overflow" },
    { 211, "object file not specified" },
};

/**
 * 実行モード: trace, logical, dump
 */
EXECMODE execmode = {false, false, false};

/**
 * アセンブル結果読み込みエラーをエラーリストに追加
 */
void addcerrlist_load()
{
    addcerrlist(ARRAYSIZE(cerr_load), cerr_load);
}

/**
 * 実行エラーをエラーリストに追加
 */
void addcerrlist_exec()
{
    addcerrlist(ARRAYSIZE(cerr_exec), cerr_exec);
}
/**
 * 指定されたファイルからアセンブル結果を読み込む
 */
bool loadassemble(const char *file)
{
    FILE *fp;
    bool stat = true;

    assert(file != NULL);
    if((fp = fopen(file, "r")) == NULL) {
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

/**
 * プログラムレジスタ（PR）を表す文字列を返す
 **/
char *pr2str(WORD pr) {
    char *str = malloc_chk(CERRSTRSIZE + 1, "pr2str.pr");

    sprintf(str, "PR:#%04X", pr);
    return str;
}

/**
 * 標準入力から文字データを読込（SVC 1）
 */
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
            setcerr(208, NULL);    /* SVC input - memory overflow */
            break;
        }
        sys->memory[sys->cpu->gr[1]+i] = *(buffer + i);
    }
    sys->memory[sys->cpu->gr[2]] = i + 1;
    FREE(buffer);
}

/**
 * 標準出力へ文字データを書出（SVC 2）
 */
void svcout()
{
    int i;
    WORD w;

    for(i = 0; i < sys->memory[sys->cpu->gr[2]]; i++) {
        if(sys->cpu->gr[1] + i > execptr->end) {
            setcerr(209, NULL);    /* SVC output - memory overflow */
            return;
        }
        /* 「文字の組」の符号表に記載された文字と、改行（CR）／タブを表示 */
        /* それ以外の文字は、「.」で表す */
        if(((w = sys->memory[sys->cpu->gr[1]+i]) >= 0x20 && w <= 0x7E) || w == 0xA || w == '\t') {
            putchar((char)w);
        } else {
            putchar('.');
        }
    }
}

/**
 * ロード／論理積／論理和／排他的論理和のフラグ設定。OFは常に0
 */
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

/**
 * WORD値からr/r1を取得
 */
WORD get_r_r1(WORD oprx)
{
    WORD r;
    if((r = ((oprx & 0x00F0) >>4)) > GRSIZE - 1) {
        setcerr(205, pr2str(sys->cpu->pr));    /* r/r1 in word #1 - not GR */
        return 0x0;
    }
    return r;
}

/**
 * WORD値からx/r2を取得
 */
WORD get_x_r2(WORD oprx)
{
    WORD x;
    if((x = (oprx & 0x000F)) > GRSIZE - 1) {
        setcerr(206, pr2str(sys->cpu->pr));    /* r/r1 in word #1 - not GR */
        return 0x0;
    }
    return x;
}

/**
 * 2つのWORD値からadr[,x]を取得
 */
WORD get_adr_x(WORD adr, WORD oprx)
{
    WORD a = adr, x;
    if((x = get_x_r2(oprx)) > 0) {
        a += sys->cpu->gr[x];
    }
    return a;
}


/**
 * 2つのWORD値からadr[,x]のアドレスに格納されている内容を取得
 */
WORD get_val_adr_x(WORD adr, WORD oprx)
{
    WORD a;
    if((a = get_adr_x(adr, oprx)) >= sys->memsize) {
        setcerr(207, pr2str(sys->cpu->pr + 1));    /* address in word #2 - out of memory */
        return 0x0;
    }
    return sys->memory[a];
}

/**
 * NOP命令。語長1（OPのみ）
 */
void nop()
{
    sys->cpu->pr += 1;
}

/**
 * LD命令 - オペランドr,adr,x。語長2
 */
void ld_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    setfr(sys->cpu->gr[get_r_r1(w[0])] = get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

/**
 * LD命令 - オペランドr1,r2。語長1
 */
void ld_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    setfr(sys->cpu->gr[get_r_r1(w[0])] = sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

/**
 * ST命令。語長2
 */
void st()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->memory[get_adr_x(w[1], w[0])] = sys->cpu->gr[get_r_r1(w[0])];
    sys->cpu->pr += 2;
}

/**
 * LAD命令。語長2
 */
void lad()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->cpu->gr[get_r_r1(w[0])] = get_adr_x(w[1], w[0]);
    sys->cpu->pr += 2;
}

/**
 * ADDA命令のテンプレート
 * 汎用レジスタrに値valを算術加算
 */
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

/**
 * ADDA命令 - オペランドr,adr,x。語長2
 */
void adda_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    adda(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

/**
 * ADDA命令 - オペランドr1,r2。語長1
 */
void adda_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    adda(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

/**
 * SUBA命令 - オペランドr,adr,x。語長2
 */
void suba_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    adda(get_r_r1(w[0]), ~(get_val_adr_x(w[1], w[0])) + 1);
    sys->cpu->pr += 2;
}

/**
 * SUBA命令 - オペランドr1,r2。語長1
 */
void suba_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    adda(get_r_r1(w[0]), ~(sys->cpu->gr[get_x_r2(w[0])]) + 1);
    sys->cpu->pr += 1;
}

/**
 * ADDL命令のテンプレート
 * 汎用レジスタrに値valを論理加算
 */
void addl(WORD r, WORD val)
{
    long tmp;
    sys->cpu->fr = 0x0;

    if((tmp = sys->cpu->gr[r] + val) < 0 || tmp > 65535) {
        sys->cpu->fr += OF;
    }
    if(((sys->cpu->gr[r] = (WORD)(tmp & 0xFFFF)) & 0x8000) == 0x8000) {
        sys->cpu->fr += SF;
    } else if(sys->cpu->gr[r] == 0x0) {
        sys->cpu->fr += ZF;
    }
}

/**
 * ADDL命令 - オペランドr,adr,x。語長2
 */
void addl_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    addl(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

/**
 * ADDL命令 - オペランドr1,r2。語長1
 */
void addl_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    addl(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

/**
 * SUBL命令 - オペランドr,adr,x。語長2
 */
void subl_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    addl(get_r_r1(w[0]), ~(get_val_adr_x(w[1], w[0])) + 1);
    sys->cpu->pr += 2;
}

/**
 * SUBL命令 - オペランドr1,r2。語長1
 */
void subl_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    addl(get_r_r1(w[0]), ~(sys->cpu->gr[get_x_r2(w[0])]) + 1);
    sys->cpu->pr += 1;
}

/**
 * AND命令 - オペランドr,adr,x。語長2
 */
void and_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    setfr(sys->cpu->gr[get_r_r1(w[0])] &= get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

/**
 * AND命令 - オペランドr1,r2。語長1
 */
void and_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    setfr(sys->cpu->gr[get_r_r1(w[0])] &= sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

/**
 * OR命令 - オペランドr,adr,x。語長2
 */
void or_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    setfr(sys->cpu->gr[get_r_r1(w[0])] |= get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

/**
 * OR命令 - オペランドr1,r2。語長1
 */
void or_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    setfr(sys->cpu->gr[get_r_r1(w[0])] |= sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

/**
 * XOR命令 - オペランドr,adr,x。語長2
 */
void xor_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    setfr(sys->cpu->gr[get_r_r1(w[0])] ^= get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

/**
 * XOR命令 - オペランドr1,r2。語長1
 */
void xor_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    setfr(sys->cpu->gr[get_r_r1(w[0])] ^= sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

/**
 * CPA命令のテンプレート
 * 汎用レジスタrの内容と値valを算術比較
 */
void cpa(WORD r, WORD val)
{
    sys->cpu->fr = 0x0;
    if((short)sys->cpu->gr[r] < (short)val) {
        sys->cpu->fr = SF;
    } else if(sys->cpu->gr[r] == val) {
        sys->cpu->fr = ZF;
    }
}

/**
 * CPA命令 - オペランドr,adr,x。語長2
 */
void cpa_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    cpa(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

/**
 * CPA命令 - オペランドr1,r2。語長1
 */
void cpa_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    cpa(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

/**
 * CPL命令のテンプレート
 * 汎用レジスタrの内容と値valを論理比較
 */
void cpl(WORD r, WORD val)
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    sys->cpu->fr = 0x0;
    if(sys->cpu->gr[r] < val) {
        sys->cpu->fr = SF;
    } else if(sys->cpu->gr[r] == val) {
        sys->cpu->fr = ZF;
    }
}


/**
 * CPL命令 - オペランドr,adr,x。語長2
 */
void cpl_r_adr_x()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    cpl(get_r_r1(w[0]), get_val_adr_x(w[1], w[0]));
    sys->cpu->pr += 2;
}

/**
 * CPL命令 - オペランドr1,r2。語長1
 */
void cpl_r1_r2()
{
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    cpl(get_r_r1(w[0]), sys->cpu->gr[get_x_r2(w[0])]);
    sys->cpu->pr += 1;
}

/**
 * SLA命令 - オペランドr,adr,x。語長2
 * 算術演算なので、第15ビットは送り出されない
 */
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

/**
 * SRA命令 - オペランドr,adr,x。語長2
 * 算術演算なので、第15ビットは送り出されない
 * 空いたビット位置には符号と同じものが入る
 */
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

/**
 * SLL命令 - オペランドr,adr,x。語長2
 */
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

/**
 * SRL命令 - オペランドr,adr,x。語長2
 */
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

/**
 * JPL命令。語長2
 */
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

/**
 * JMI命令。語長2
 */
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

/**
 * JNZ命令。語長2
 */
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

/**
 * JZE命令。語長2
 */
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

/**
 * JOV命令。語長2
 */
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

/**
 * JUMP命令。語長2
 */
void jump()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->cpu->pr = get_adr_x(w[1], w[0]);
}

/**
 * PUSH命令。語長2
 */
void push()
{
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    assert(sys->cpu->sp > execptr->end && sys->cpu->sp <= sys->memsize);
    sys->memory[--(sys->cpu->sp)] = get_adr_x(w[1], w[0]);
    sys->cpu->pr += 2;
}

/**
 * POP命令。語長1
 */
void pop()
{
    assert(sys->cpu->sp > execptr->end && sys->cpu->sp <= sys->memsize);
    WORD w[1];
    w[0] = sys->memory[sys->cpu->pr];
    sys->cpu->gr[get_r_r1(w[0])] = sys->memory[(sys->cpu->sp)++];
    sys->cpu->pr += 1;
}

/**
 * CALL命令。語長2
 */
void call()
{
    assert(sys->cpu->sp > execptr->end && sys->cpu->sp <= sys->memsize);
    WORD w[2];
    w[0] = sys->memory[sys->cpu->pr];
    w[1] = sys->memory[sys->cpu->pr + 1];
    sys->memory[--(sys->cpu->sp)] = sys->cpu->pr + 1;
    sys->cpu->pr = get_adr_x(w[1], w[0]);
}

/**
 * RET命令。語長1（OPのみ）
 */
void ret()
{
    assert(sys->cpu->sp <= sys->memsize);
    if(sys->cpu->sp == sys->memsize) {
        execptr->stop = true;
    } else if(sys->cpu->sp < sys->memsize) {
        sys->cpu->pr = sys->memory[(sys->cpu->sp)++] + 1;
    }
}

/**
 * SVC命令。語長2
 */
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

/**
 * 仮想マシンCOMET IIの実行
 */
void exec()
{
    clock_t clock_begin, clock_end;
    void (*cmdptr)();

    create_code_type();                             /* 命令のコードとタイプがキーのハッシュ表を作成 */
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
        /* プログラムレジスタとスタックポインタをチェック */
        if(sys->cpu->pr >= sys->memsize || sys->cpu->sp > sys->memsize || sys->cpu->sp <= execptr->end) {
            if(sys->cpu->pr >= sys->memsize) {
                setcerr(201, pr2str(sys->cpu->pr));        /* Program Register (PR) - memory overflow */
            } else if(sys->cpu->sp <= execptr->end) {
                setcerr(202, pr2str(sys->cpu->pr));        /* Stack Pointer (SP) - stack overflow */
            } else if(sys->cpu->sp > sys->memsize) {
                setcerr(203, pr2str(sys->cpu->pr));        /* Stack Pointer (SP) - stack underflow */
            }
            goto execfin;
        }
        /* コードから命令を取得 */
        /* 取得できない場合はエラー終了 */
        if((cmdptr = getcmdptr(sys->memory[sys->cpu->pr] & 0xFF00)) == NULL) {
            setcerr(204, pr2str(sys->cpu->pr));            /* OP in word #1 - not command code */
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
    free_code_type();                              /* 命令のコードとタイプがキーのハッシュ表を解放 */
    if(cerr->num > 0) {
        fprintf(stderr, "Execute error - %d: %s\n", cerr->num, cerr->msg);
    }
}
