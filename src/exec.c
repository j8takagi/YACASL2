#include "casl2.h"
#include "exec.h"

/* 標準入力から文字データを読込（SVC 1） */
void svcin()
{
    int i;
    char *buffer = malloc(INSIZE + 1);

    if(fgets(buffer, INSIZE, stdin) == NULL) {
        memory[GR[1]] = 0x0;
        memory[GR[2]] = 0x0;
        return;
    }
    for(i = 0; i < GR[1] && i < INSIZE; i++) {
        if(*(buffer + i) == '\0' || *(buffer + i) == '\n') {
            --i;
            break;
        }
        if(GR[1] + i >= memsize - 1) {
            setcerr(202, NULL);    /* SVC input - out of Input memory */
            break;
        }
        memory[GR[1]+i] = *(buffer + i);
    }
    memory[GR[2]] = i + 1;
}

/* 標準出力へ文字データを書出（SVC 2） */
void svcout()
{
    int i;
    WORD w;

    for(i = 0; i < memory[GR[2]]; i++) {
        if(GR[1] + i >= memsize - 1) {
            setcerr(203, NULL);    /* SVC output - out of Comet II memory */
            return;
        }
        if((w = memory[GR[1]+i]) == '\0') {
            break;
        }
        /* 「文字の組」の符号表に記載された文字と、改行（CR）／タブを表示 */
        /* それ以外の文字は、「.」で表す */
        if((w >= 0x20 && w <= 0x7E) || w == 0xA || w == '\t') {
            putchar((char)w);
        } else {
            putchar('.');
        }
    }
}

/* ロード／論理積／論理和／排他的論理和のフラグ設定。OFは常に0 */
void setfr(WORD val)
{
    FR = 0x0;
    /* 第15ビットが1のとき、SFは1 */
    if((val & 0x8000) > 0x0) {
        FR += SF;
    }
    /* 演算結果が0のとき、ZFは1 */
    if(val == 0x0) {
        FR += ZF;
    }
}

/* 算術加算。フラグを設定して値を返す */
WORD adda(WORD val0, WORD val1)
{
    WORD res;
    long temp;
    FR = 0x0;

    temp = (signed short)val0 + (signed short)val1;
    if(temp > 32767 || temp < -32768) {
        FR += OF;
    }
    if(((res = (WORD)(temp & 0xFFFF)) & 0x8000) == 0x8000) {
        FR += SF;
    } else if(res == 0x0) {
        FR += ZF;
    }
    return res;
}

/* 算術減算。フラグを設定して値を返す */
WORD suba(WORD val0, WORD val1)
{
    return adda(val0, (~val1 + 1));
}

/* 論理加算。フラグを設定して値を返す */
WORD addl(WORD val0, WORD val1)
{
    long temp;
    WORD res;
    FR = 0x0;

    if((temp = val0 + val1) < 0 || temp > 65535) {
        FR += OF;
    }
    if(((res = (WORD)(temp & 0xFFFF)) & 0x8000) == 0x8000) {
        FR += SF;
    } else if(res == 0x0) {
        FR += ZF;
    }
    return res;
}

/* 論理減算。フラグを設定して値を返す */
WORD subl(WORD val0, WORD val1)
{
    return addl(val0, (~val1 + 1));
}

/* 算術比較のフラグ設定。OFは常に0 */
void cpa(WORD val0, WORD val1)
{
    FR = 0x0;
    if((short)val0 < (short)val1) {
        FR = SF;
    } else if(val0 == val1) {
        FR = ZF;
    }
}

/* 論理比較のフラグ設定。OFは常に0 */
void cpl(WORD val0, WORD val1)
{
    FR = 0x0;
    if(val0 < val1) {
        FR = SF;
    } else if(val0 == val1) {
        FR = ZF;
    }
}

/* 算術左シフト。フラグを設定して値を返す。 */
/* 算術演算なので、第15ビットは送り出されない */
WORD sla(WORD val0, WORD val1)
{
    WORD sign, res, last;
    int i;

    FR = 0x0;
    sign = val0 & 0x8000;
    res = val0 & 0x7FFF;
    for(i = 0; i < val1; i++) {
        last = res & 0x4000;
        res <<= 1;
    }
    res = sign | res;
    /* OFに、レジスタから最後に送り出されたビットの値を設定 */
    if(last > 0x0) {
        FR += OF;
    }
    /* 符号（第15ビット）が1のとき、SFは1 */
    if(sign > 0x0) {
        FR += SF;
    }
    /* 演算結果が0のとき、ZFは1 */
    if(res == 0x0) {
        FR += ZF;
    }
    return res;
}

/* 算術右シフト。フラグを設定して値を返す */
/* 算術演算なので、第15ビットは送り出されない */
/* 空いたビット位置には符号と同じものが入る */
WORD sra(WORD val0, WORD val1)
{
    WORD sign, res, last;
    int i;

    FR = 0x0;
    sign = val0 & 0x8000;
    res = val0 & 0x7FFF;
    for(i = 0; i < val1; i++) {
        last = res & 0x1;
        res >>= 1;
        if(sign > 0) {
            res |= 0x4000;
        }
    }
    res = sign | res;
    /* OFに、レジスタから最後に送り出されたビットの値を設定 */
    if(last > 0x0) {
        FR += OF;
    }
    /* 符号（第15ビット）が1のとき、SFは1 */
    if(sign > 0x0) {
        FR += SF;
    }
    /* 演算結果が0のとき、ZFは1 */
    if(res == 0x0) {
        FR += ZF;
    }
    return res;
}

/* 論理左シフト。フラグを設定して値を返す */
WORD sll(WORD val0, WORD val1)
{
    WORD res = val0, last;
    int i;

    FR = 0x0;
    for(i = 0; i < val1; i++) {
        last = res & 0x8000;
        res <<= 1;
    }
    /* OFに、レジスタから最後に送り出されたビットの値を設定 */
    if(last > 0x0) {
        FR += OF;
    }
    /* 第15ビットが1のとき、SFは1 */
    if((res & 0x8000) > 0x0) {
        FR += SF;
    }
    /* 演算結果が0のとき、ZFは1 */
    if(res == 0x0) {
        FR += ZF;
    }
    return res;
}

/* 論理右シフト。フラグを設定して値を返す */
WORD srl(WORD val0, WORD val1)
{
    WORD res = val0, last;
    int i;

    FR = 0x0;
    for(i = 0; i < val1; i++) {
        last = res & 0x0001;
        res >>= 1;
    }
    /* OFに、レジスタから最後に送り出されたビットの値を設定 */
    if(last > 0x0) {
        FR += OF;
    }
    /* 第15ビットが1のとき、SFは1 */
    if((res & 0x8000) > 0x0) {
        FR += SF;
    }
    /* 演算結果が0のとき、ZFは1 */
    if(res == 0x0) {
        FR += ZF;
    }
    return res;
}

/* COMET II仮想マシンのリセット */
void reset()
{
    int i;
    for(i = 0; i < REGSIZE; i++) {
        GR[i] = 0x0;
    }
    SP = PR = FR = 0x0;
    memory = malloc(memsize);
    for(i = 0; i < memsize; i++) {
        memory[i] = 0x0;
    }
}

/* コードの実行 */
void exec()
{
    WORD op, r_r1, x_r2, val;
    CMDTYPE cmdtype;
    char *errpr = malloc(8);
    clock_t clock_begin, clock_end;

    if((&execmode)->tracemode) {
        fprintf(stdout, "\nExecuting machine codes\n");
    }
    /* フラグレジスタの初期値設定 */
    FR = 0x0;
    SP = memsize;
    PR = startptr;
    if(create_code_type() == false) {
        goto execerr;
    }
    /* 機械語の実行 */
    for (; ; ) {
        clock_begin = clock();
        /* プログラムレジスタのアドレスが主記憶の範囲外の場合はエラー */
        if(PR >= memsize) {
            sprintf(errpr, "PR:#%04X", PR);
            setcerr(204, errpr);    /* Program Register (PR) - out of COMET II memory */
        }
        /* スタック領域のアドレスが主記憶の範囲外の場合はエラー */
        if(SP > memsize) {
            sprintf(errpr, "PR:#%04X", PR);
            setcerr(207, errpr);    /* Stack Pointer (SP) - out of COMET II memory */
        }
        /* スタック領域を確保できない場合はエラー */
        if(SP <= endptr) {
            sprintf(errpr, "PR:#%04X", PR);
            setcerr(205, errpr);    /* Stack Pointer (SP) - cannot allocate stack buffer */
        }
        op = memory[PR] & 0xFF00;
        cmdtype = getcmdtype(op);
        r_r1 = (memory[PR] >> 4) & 0xF;
        x_r2 = memory[PR] & 0xF;
        /* エラー発生時は終了 */
        if(cerrno > 0) {
            goto execerr;
        }
        if((&execmode)->tracemode){
            fprintf(stdout, "#%04X: Register::::\n", PR);
            dspregister();
        }
        if((&execmode)->dumpmode){
            fprintf(stdout, "#%04X: Memory::::\n", PR);
            dumpmemory();
        }
        if((&execmode)->dumpmode || (&execmode)->tracemode) {
            fprintf(stdout, "\n");
        }
        PR++;
        /* 処理対象の値を取得 */
        if(cmdtype == R1_R2) {
            assert(x_r2 < REGSIZE);
            val = GR[x_r2];
        }
        else if(cmdtype ==  R_ADR_X || cmdtype == R_ADR_X_ || cmdtype == ADR_X) {
            /* 実効アドレス（値または値が示す番地）を取得  */
            val = memory[PR++];
            /* 指標アドレスを加算  */
            if(x_r2 > 0x0) {
                val += GR[x_r2];
            }
            /* ロード／算術論理演算命令／比較演算命令では、アドレスに格納されている内容を取得 */
            if(cmdtype == R_ADR_X_) {
                if(val >= memsize) {
                    sprintf(errpr, "PR:#%04X", PR-1);
                    setcerr(206, errpr);    /* Address - out of COMET II memory */
                    goto execerr;
                }
                val = memory[val];
            }
        }
        /* 主オペランドが1〜4の場合、第2ビットを無視 */
        if(op >= 0x1000 && op <= 0x4FFF) {
            op &= 0xFB00;
        }
        /* 命令ごとの処理を実行 */
        switch(op)
        {
        case 0x0:  /* NOP */
            break;
        case 0x1000:    /* LD */
            setfr(GR[r_r1] = val);
            break;
        case 0x1100:    /* ST */
            memory[val] = GR[r_r1];
            break;
        case 0x1200:    /* LAD */
            GR[r_r1] = val;
            break;
        case 0x2000:  /* ADDA */
            GR[r_r1] = adda(GR[r_r1], val);
            break;
        case 0x2100:  /* SUBA */
            GR[r_r1] = suba(GR[r_r1], val);
            break;
        case 0x2200:  /* ADDL */
            GR[r_r1] = addl(GR[r_r1], val);
            break;
        case 0x2300:  /* SUBL */
            GR[r_r1] = subl(GR[r_r1], val);
            break;
        case 0x3000:  /* AND */
            setfr(GR[r_r1] &= val);
            break;
        case 0x3100:  /* OR */
            setfr(GR[r_r1] |= val);
            break;
        case 0x3200:  /* XOR */
            setfr(GR[r_r1] ^= val);
            break;
        case 0x4000:  /* CPA */
            cpa(GR[r_r1], val);
            break;
        case 0x4100:  /* CPL */
            cpl(GR[r_r1], val);
            break;
        case 0x5000:  /* SLA */
            GR[r_r1] = sla(GR[r_r1], val);
            break;
        case 0x5100:  /* SRA */
            GR[r_r1] = sra(GR[r_r1], val);
            break;
        case 0x5200:  /* SLL */
            GR[r_r1] = sll(GR[r_r1], val);
            break;
        case 0x5300:  /* SRL */
            GR[r_r1] = srl(GR[r_r1], val);
            break;
        case 0x6100:  /* JMI */
            if((FR & SF) > 0) {
                PR = val;
            }
            break;
        case 0x6200:  /* JNZ */
            if((FR & ZF) == 0) {
                PR = val;
            }
            break;
        case 0x6300:  /* JZE */
            if((FR & ZF) > 0) {
                PR = val;
            }
            break;
        case 0x6400:  /* JUMP */
            PR = val;
            break;
        case 0x6500:  /* JPL */
            if((FR & (SF | ZF)) == 0) {
                PR = val;
            }
            break;
        case 0x6600:  /* JOV */
            if((FR & OF) > 0) {
                PR = val;
            }
            break;
        case 0x7000:  /* PUSH */
            assert(SP > endptr && SP <= memsize);
            memory[--SP] = val;
            break;
        case 0x7100:  /* POP */
            assert(SP > endptr && SP <= memsize);
            GR[r_r1] = memory[SP++];
            break;
        case 0x8000:  /* CALL */
            assert(SP > endptr && SP <= memsize);
            memory[--SP] = PR;
            PR = val;
            break;
        case 0x8100:  /* RET */
            assert(SP > endptr && SP <= memsize);
            if(SP == memsize) {
                return;
            } else {
                PR = memory[SP++];
                break;
            }
        case 0xf000:  /* SVC */
            switch(val)
            {
            case 0x0: /* EXIT */
                return;
            case 0x1: /* IN */
                svcin();
                break;
            case 0x2: /* OUT */
                svcout();
                break;
            }
        default:
            break;
        }
        do {
            clock_end = clock();
        } while(clock_end - clock_begin < CLOCKS_PER_SEC / clocks);
/*        printf("PR:%04X; time: %f\n", PR, (double)((clock_end - clock_begin) * CLOCKS_PER_SEC)); */
    }
execerr:
    fprintf(stderr, "Execute error - %d: %s\n", cerrno, cerrmsg);
}
