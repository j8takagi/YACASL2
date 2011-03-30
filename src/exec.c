#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "exec.h"
#include "cerr.h"

/**
 * 実行エラーの定義
 */
static CERR cerr_exec[] = {
    { 202, "SVC input - out of Input memory" },
    { 203, "SVC output - out of COMET II memory" },
    { 204, "Program Register (PR) - out of COMET II memory" },
    { 205, "Stack Pointer (SP) - cannot allocate stack buffer" },
    { 206, "Address - out of COMET II memory" },
    { 207, "Stack Pointer (SP) - out of COMET II memory" },
    { 209, "not GR in operand x" },
    { 210, "not command code of COMET II" },
};

/**
 * アセンブル結果読み込みエラーの定義
 */
static CERR cerr_load[] = {
    { 201, "Loading - full of COMET II memory" },
    { 208, "object file is not specified" },
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
bool loadassemble(char *file)
{
    FILE *fp;
    bool status = true;

    assert(file != NULL);
    if((fp = fopen(file, "r")) == NULL) {
        perror(file);
        return false;
    }
    execptr->end = execptr->start +
        fread(sys->memory, sizeof(WORD), sys->memsize - execptr->start, fp);
    if(execptr->end == sys->memsize) {
        setcerr(201, file);    /* Loading - full of COMET II memory */
        fprintf(stderr, "Load error - %d: %s\n", cerr->num, cerr->msg);
        status = false;
    }
    fclose(fp);
    return status;
}

/**
 * プログラムレジスタ（PR）を表す文字列を返す
 **/
static char *pr2str(WORD pr) {
    char *str = malloc_chk(CERRSTRSIZE + 1, "pr2str.pr");

    sprintf(str, "PR:#%04X", pr);
    return str;
}

/**
 * 標準入力から文字データを読込（SVC 1）
 */
static void svcin()
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
        if(sys->cpu->gr[1] + i >= sys->memsize - 1) {
            setcerr(202, NULL);    /* SVC input - out of Input memory */
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
static void svcout()
{
    int i;
    WORD w;

    for(i = 0; i < sys->memory[sys->cpu->gr[2]]; i++) {
        if(sys->cpu->gr[1] + i >= sys->memsize - 1) {
            setcerr(203, NULL);    /* SVC output - out of Comet II memory */
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
static void setfr(WORD val)
{
    sys->cpu->fr = 0x0;
    /* 第15ビットが1のとき、SFは1 */
    if((val & 0x8000) == 0x8000) {
        sys->cpu->fr += SF;
    }
    /* 演算結果が0のとき、ZFは1 */
    if(val == 0x0) {
        sys->cpu->fr += ZF;
    }
}

/**
 * NOP命令
 */
void nop(const WORD r, const WORD v)
{

}

/**
 * LD命令
 */
void ld(const WORD r, const WORD v)
{
    setfr(sys->cpu->gr[r] = v);
}

/**
 * ST命令
 */
void st(const WORD r, const WORD v)
{
    sys->memory[v] = sys->cpu->gr[r];
}

/**
 * LAD命令
 */
void lad(const WORD r, const WORD v)
{
    sys->cpu->gr[r] = v;
}

/**
 * ADDA命令
 */
void adda(const WORD r, const WORD v)
{
    long tmp;

    sys->cpu->fr = 0x0;
    /* 引数の値を16ビット符号付整数として加算し、オーバーフローをチェック */
    assert(sizeof(short) * 8 == 16 && (short)0xFFFF == -1);
    if((tmp = (short)(sys->cpu->gr[r]) + (short)v) > 32767 || tmp < -32768) {
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
 * SUBA命令
 */
void suba(const WORD r, const WORD v)
{
    adda(r, (~v + 1));
}

/**
 * ADDL命令
 */
void addl(const WORD r, const WORD v)
{
    long tmp;
    sys->cpu->fr = 0x0;

    if((tmp = sys->cpu->gr[r] + v) < 0 || tmp > 65535) {
        sys->cpu->fr += OF;
    }
    if(((sys->cpu->gr[r] = (WORD)(tmp & 0xFFFF)) & 0x8000) == 0x8000) {
        sys->cpu->fr += SF;
    } else if(sys->cpu->gr[r] == 0x0) {
        sys->cpu->fr += ZF;
    }
}

/**
 * SUBL命令
 */
void subl(const WORD r, const WORD v)
{
    addl(r, (~v + 1));
}

/**
 * AND命令
 */
void and(const WORD r, const WORD v)
{
    setfr(sys->cpu->gr[r] &= v);
}

/**
 * OR命令
 */
void or(const WORD r, const WORD v)
{
    setfr(sys->cpu->gr[r] |= v);
}

/**
 * XOR命令
 */
void xor(const WORD r, const WORD v)
{
    setfr(sys->cpu->gr[r] ^= v);
}

/**
 * CPA命令
 */
void cpa(const WORD r, const WORD v)
{
    sys->cpu->fr = 0x0;
    if((short)sys->cpu->gr[r] < (short)v) {
        sys->cpu->fr = SF;
    } else if(sys->cpu->gr[r] == v) {
        sys->cpu->fr = ZF;
    }
}

/**
 * CPL命令
 */
void cpl(const WORD r, const WORD v)
{
    sys->cpu->fr = 0x0;
    if(sys->cpu->gr[r] < v) {
        sys->cpu->fr = SF;
    } else if(sys->cpu->gr[r] == v) {
        sys->cpu->fr = ZF;
    }
}


/**
 * SLA命令。算術演算なので、第15ビットは送り出されない
 */
void sla(const WORD r, const WORD v)
{
    WORD sign, last = 0x0;
    int i;

    sys->cpu->fr = 0x0;
    sign = sys->cpu->gr[r] & 0x8000;
    sys->cpu->gr[r] &= 0x7FFF;
    for(i = 0; i < v; i++) {
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
}

/**
 * SRA命令
 * 算術演算なので、第15ビットは送り出されない
 * 空いたビット位置には符号と同じものが入る
 */
void sra(const WORD r, const WORD v)
{
    WORD sign, last = 0x0;
    int i;

    sys->cpu->fr = 0x0;
    sign = sys->cpu->gr[r] & 0x8000;
    sys->cpu->gr[r] &= 0x7FFF;
    for(i = 0; i < v; i++) {
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
}

/**
 * SLL命令
 */
void sll(const WORD r, const WORD v)
{
    WORD last = 0x0;
    int i;

    sys->cpu->fr = 0x0;
    for(i = 0; i < v; i++) {
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
}

/**
 * SRL命令
 */
void srl(const WORD r, const WORD v)
{
    WORD last = 0x0;
    int i;

    sys->cpu->fr = 0x0;
    for(i = 0; i < v; i++) {
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
}

/**
 * JMI命令
 */
void jmi(const WORD r, const WORD v)
{
    if((sys->cpu->fr & SF) > 0) {
        sys->cpu->pr = v;
    }
}

/**
 * JNZ命令
 */
void jnz(const WORD r, const WORD v)
{
    if((sys->cpu->fr & ZF) == 0) {
        sys->cpu->pr = v;
    }
}

/**
 * JZE命令
 */
void jze(const WORD r, const WORD v)
{
    if((sys->cpu->fr & ZF) > 0) {
        sys->cpu->pr = v;
    }
}

/**
 * JUMP命令
 */
void jump(const WORD r, const WORD v)
{
    sys->cpu->pr = v;
}

/**
 * JPL命令
 */
void jpl(const WORD r, const WORD v)
{
    if((sys->cpu->fr & (SF | ZF)) == 0) {
        sys->cpu->pr = v;
    }
}

/**
 * JOV命令
 */
void jov(const WORD r, const WORD v)
{
    if((sys->cpu->fr & OF) > 0) {
        sys->cpu->pr = v;
    }
}

/**
 * PUSH命令
 */
void push(const WORD r, const WORD v)
{
    assert(sys->cpu->sp > execptr->end && sys->cpu->sp <= sys->memsize);
    sys->memory[--(sys->cpu->sp)] = v;
}

/**
 * POP命令
 */
void pop(const WORD r, const WORD v)
{
    assert(sys->cpu->sp > execptr->end && sys->cpu->sp <= sys->memsize);
    sys->cpu->gr[r] = sys->memory[(sys->cpu->sp)++];
}

/**
 * CALL命令
 */
void call(const WORD r, const WORD v)
{
    assert(sys->cpu->sp > execptr->end && sys->cpu->sp <= sys->memsize);
    sys->memory[--(sys->cpu->sp)] = sys->cpu->pr;
    sys->cpu->pr = v;
}

/**
 * RET命令
 */
void ret(const WORD r, const WORD v)
{
    assert(sys->cpu->sp <= sys->memsize);
    if(sys->cpu->sp == sys->memsize) {
        execptr->stop = true;
    } else if(sys->cpu->sp < sys->memsize) {
        sys->cpu->pr = sys->memory[(sys->cpu->sp)++];
    }
}

/**
 * SVC命令
 */
void svc(const WORD r, const WORD v)
{
    switch(v)
    {
    case 0x0:
        execptr->stop = true;
        break;
    case 0x1:                   /* IN */
        svcin();
        break;
    case 0x2:                   /* OUT */
        svcout();
        break;
    }
}

/**
 * 仮想マシンCOMET IIの実行
 */
bool exec()
{
    WORD op, r_r1, x_r2, val;
    CMDTYPE cmdtype;
    void (*cmdptr)();
    clock_t clock_begin, clock_end;

    if(execmode.trace == true) {
        fprintf(stdout, "\nExecuting machine codes\n");
    }
    /* フラグレジスタの初期値設定 */
    sys->cpu->fr = 0x0;
    /* スタックポインタの初期値設定 */
    sys->cpu->sp = sys->memsize;
    /* 終了フラグの初期値設定 */
    execptr->stop = false;
    /* 機械語の実行 */
    for (sys->cpu->pr = execptr->start; ; ) {
        clock_begin = clock();
        /* プログラムレジスタのアドレスが主記憶の範囲外の場合はエラー終了 */
        if(sys->cpu->pr >= sys->memsize) {
            setcerr(204, pr2str(sys->cpu->pr));    /* Program Register (PR) - out of COMET II memory */
            goto execerr;
        }
        /* スタック領域を確保できない場合はエラー終了 */
        else if(sys->cpu->sp <= execptr->end) {
            setcerr(205, pr2str(sys->cpu->pr));    /* Stack Pointer (SP) - cannot allocate stack buffer */
            goto execerr;
        }
        /* スタック領域のアドレスが主記憶の範囲外の場合はエラー終了 */
        else if(sys->cpu->sp > sys->memsize) {
            setcerr(207, pr2str(sys->cpu->pr));    /* Stack Pointer (SP) - out of COMET II memory */
            goto execerr;
        }
        /* 命令の取り出し */
        op = sys->memory[sys->cpu->pr] & 0xFF00;
        /* 命令の解読 */
        /* 命令がCOMET II命令ではない場合はエラー終了 */
        if((cmdtype = getcmdtype(op)) == NOTCMD) {
            setcerr(210, pr2str(sys->cpu->pr));          /* not command code of COMET II */
            goto execerr;
        }
        cmdptr = getcmdptr(op);
        r_r1 = (sys->memory[sys->cpu->pr] >> 4) & 0xF;
        x_r2 = sys->memory[sys->cpu->pr] & 0xF;
        /* traceオプション指定時、レジスタを出力 */
        if(execmode.trace){
            fprintf(stdout, "#%04X: Register::::\n", sys->cpu->pr);
            dspregister();
        }
        /* dumpオプション指定時、メモリを出力 */
        if(execmode.dump){
            fprintf(stdout, "#%04X: Memory::::\n", sys->cpu->pr);
            dumpmemory();
        }
        /* traceまたはdumpオプション指定時、改行を出力 */
        if(execmode.dump || execmode.trace) {
            fprintf(stdout, "\n");
        }
        sys->cpu->pr++;
        /* オペランドの取り出し */
        if(cmdtype == R1_R2) {
            /* オペランドの数値が汎用レジスタの範囲外の場合はエラー */
            if(x_r2 > GRSIZE - 1) {
                setcerr(209, pr2str(sys->cpu->pr-1));    /* not GR in operand x */
                goto execerr;
            }
            val = sys->cpu->gr[x_r2];
        }
        else if(cmdtype ==  R_ADR_X || cmdtype == R_ADR_X_ || cmdtype == ADR_X) {
            /* オペランドの数値が汎用レジスタの範囲外の場合はエラー */
            if(x_r2 > GRSIZE - 1) {
                setcerr(209, pr2str(sys->cpu->pr-1));    /* not GR in operand x */
                goto execerr;
            }
            /* 実効アドレス（値または値が示す番地）を取得  */
            val = sys->memory[sys->cpu->pr++];
            /* 指標アドレスを加算  */
            if(x_r2 > 0x0) {
                val += sys->cpu->gr[x_r2];
            }
            /* ロード／算術論理演算命令／比較演算命令では、アドレスに格納されている内容を取得 */
            if(cmdtype == R_ADR_X_) {
                if(val >= sys->memsize) {
                    setcerr(206, pr2str(sys->cpu->pr-1));    /* Address - out of COMET II memory */
                    goto execerr;
                }
                val = sys->memory[val];
            }
        }
        /* 主オペランドが1から4の場合、第2ビットを無視 */
        if(op >= 0x1000 && op <= 0x4FFF) {
            op &= 0xFB00;
        }
        /* 命令の実行 */
        (*cmdptr)(r_r1, val);
        /* エラー発生時はエラー終了 */
        if(cerr->num > 0) {
            goto execerr;
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
    return true;
execerr:
    fprintf(stderr, "Execute error - %d: %s\n", cerr->num, cerr->msg);
    return false;
}
