#include "casl2.h"
#include "assemble.h"

/* アセンブルモード: src, label, onlylabel, asdetail, onlyassemble */
ASMODE asmode = {false, false, false, false, false};

/* 値を格納するポインタ */
WORD ptr;

/* リテラル（=付きの値）を格納するポインタ */
WORD lptr;

/* 他のプログラムで参照する入口名 */
char *prog;

/* 汎用レジスタを表す文字列「GR[0-7]」から、レジスタ番号[0-7]をWORD値で返す */
/* 文字列が汎用レジスタを表さない場合は、0xFFFFを返す */
/* is_xがtrueの場合は指標レジスタ。GR0は、COMET IIの仕様により、エラー発生 */
WORD getgr(const char *str, bool is_x)
{
    assert(str != NULL);
    WORD r;
    /* 「GR[0-7]」以外の文字列では、0xFFFFを返して終了 */
    if(!(strlen(str) == 3 && strncmp(str, "GR", 2) == 0 &&
         (*(str+2) >= '0' && *(str+2) <= '7')))
    {
        return 0xFFFF;
    }
    r = (WORD)(*(str+2) - '0');
    /* 指標レジスタとして用いることはできない */
    if(is_x == true && r == 0x0) {
        setcerr(120, NULL);    /* GR0 in operand x */
        return 0x0;
    }
    return r;
}

/* アドレスを返す */
/* アドレスには、リテラル／10進定数／16進定数／アドレス定数が含まれる */
WORD getadr(const char *prog, const char *str, PASS pass)
{
    WORD adr = 0x0;
    if(*str == '=') {
        adr = getliteral(str, pass);
    } else if((*str == '#') || isdigit(*str) || *str == '-') {
        adr = nh2word(str);
    } else {
        if(pass == SECOND && (adr = getlabel(prog, str)) == 0xFFFF) {
            if(prog != NULL) {
                setcerr(103, str);    /* label not found */
            }
        }
    }
    return adr;
}

/* WORD値wordをアドレスadrに書込 */
/* 書込に成功した場合はtrue、失敗した場合はfalseを返す */
bool writememory(WORD word, WORD adr, PASS pass)
{
    bool status = false;
    /* COMET IIメモリオーバーの場合 */
    if(adr >= memsize) {
        setcerr(119, word2n(adr));    /* out of COMET II memory */
    }
    if(cerrno == 0) {
        memory[adr] = word;
        if(pass == SECOND && (&asmode)->asdetail == true) {
            fprintf(stdout, "\t#%04X\t#%04X\n", adr, word);
        }
        status = true;
    }
    return status;
}

/* 定数の前に等号（=）をつけて記述されるリテラルを返す */
/* リテラルには、10進定数／16進定数／文字定数が含まれる */
WORD getliteral(const char *str, PASS pass)
{
    WORD adr = lptr;
    assert(*str == '=');
    if(*(++str) == '\'') {    /* 文字定数 */
        writestr(str, true, pass);
    } else {
        writememory(nh2word(str), lptr++, pass);
    }
    return adr;
}

/* ' 'で囲まれた文字定数をメモリに書込 */
/* DC命令とリテラルで使い、リテラルの場合はリテラル領域に書込 */
void writestr(const char *str, bool literal, PASS pass)
{
    assert(cerrno == 0 && *str == '\'');
    const char *p = str + 1;
    bool lw = false;

    for(; ;) {
        /* 閉じ「'」がないまま文字列が終了した場合 */
        if(*p == '\0') {
            setcerr(123, str);    /* illegal string */
            break;
        }
        /* 「'」の場合、次の文字が「'」でない場合は正常終了 */
        if(*p == '\'' && *(++p) != '\'') {
            break;
        } else if(literal == true && lw == true) {
            setcerr(124, str);    /* more than one character in literal */
            break;
        }
        /*リテラルの場合はリテラル領域に書込 */
        if(literal == true) {
            writememory(*(p++), lptr++, pass);
            lw = true;
        } else {
            writememory(*(p++), ptr++, pass);
        }
    }
}

/* アセンブラ命令DCをメモリに書込 */
void writeDC(const char *str, PASS pass)
{
    WORD adr = 0x0;
    if(*str == '\'') {
        writestr(str, false, pass);
    } else {
        if(*str == '#' || isdigit(*str) || *str == '-') {
            adr = nh2word(str);
        } else {
            if(pass == SECOND && (adr = getlabel(prog, str)) == 0xFFFF) {
                setcerr(103, str);    /* label not found */
            }
        }
        writememory(adr, ptr++, pass);
    }
}

/* 命令がアセンブラ命令の場合は処理を実行 */
/* 実行に成功した場合はtrue、それ以外の場合はfalseを返す */
bool assemblecmd(const CMDLINE *cmdl, PASS pass)
{
    int i = 0;
    CASLCMD cmd = 0;
    bool status = false;
    CMDARRAY ascmd[] = {
        { START, 0, 1, "START" },
        { END, 0, 0, "END" },
        { DC, 1, OPDSIZE, "DC" },
        { DS, 1, 1, "DS" },
        { 0, 0, 0, NULL }
    };

    do {
        if(strcmp(cmdl->cmd, ascmd[i].cmd) == 0) {
            if(cmdl->opd->opdc < ascmd[i].opdc_min || cmdl->opd->opdc > ascmd[i].opdc_max) {
                setcerr(106, NULL);    /* operand count mismatch */
                return false;
            }
            cmd = ascmd[i].cmdid;
            break;
        }
    } while(ascmd[++i].cmdid != 0);
    /* アセンブラ命令 */
    switch(cmd)
    {
    case START:
        if(cmdl->label == NULL) {
            setcerr(107, NULL);    /* no label in START */
            return false;
        }
        /* プログラム名の設定 */
        prog = strdup(cmdl->label);
        /* オペランドがある場合、実行開始番地を設定 */
        if(pass == SECOND && cmdl->opd->opdc == 1) {
            if((startptr = getlabel(prog, cmdl->opd->opdv[0])) == 0xFFFF) {
                setcerr(103, cmdl->opd->opdv[0]);    /* label not found */
            }
        }
        status = true;
        break;
    case END:
        /* リテラル領域の設定 */
        if(pass == FIRST) {
            lptr = ptr;
        }
        /* 実行終了番地と次のプログラムの実行開始番地を設定 */
        else if(pass == SECOND) {
            endptr = lptr;
        }
        prog = NULL;
        status = true;
        break;
    case DS:
        for(i = 0; i < atoi(cmdl->opd->opdv[0]); i++) {
            writememory(0x0, ptr++, pass);
            if(cerrno > 0) {
                return false;
            }
        }
        status = true;
        break;
    case DC:
        for(i = 0; i < cmdl->opd->opdc; i++) {
            writeDC(cmdl->opd->opdv[i], pass);
            if(cerrno > 0) {
                return false;
            }
        }
        status = true;
        break;
    default:
        return false;
    }
    if(cerrno > 0) {
        status = false;
    }
    return status;
}

/* 命令がマクロ命令の場合はメモリに書込 */
/* 書込に成功した場合はtrue、それ以外の場合はfalseを返す */
bool macrocmd(const CMDLINE *cmdl, PASS pass)
{
    int i = 0;
    CASLCMD cmd;
    bool status = false;
    CMDARRAY macrocmd[] = {
        { IN, 2, 2, "IN" },
        { OUT, 2, 2, "OUT" },
        { RPUSH, 0, 0, "RPUSH" },
        { RPOP, 0, 0, "RPOP" },
        { 0, 0, 0, NULL }
    };

    do {
        if(strcmp(cmdl->cmd, macrocmd[i].cmd) == 0) {
            if(cmdl->opd->opdc < macrocmd[i].opdc_min || cmdl->opd->opdc > macrocmd[i].opdc_max) {
                setcerr(106, NULL);    /* operand count mismatch */
                return false;
            }
            cmd = macrocmd[i].cmdid;
            break;
        }
    } while(macrocmd[++i].cmdid != 0);
    switch(cmd)
    {
    case IN:
        status = writeIN(cmdl->opd->opdv[0], cmdl->opd->opdv[1], pass);
        break;
    case OUT:
        status = writeOUT(cmdl->opd->opdv[0], cmdl->opd->opdv[1], pass);
        break;
    case RPUSH:
        status = writeRPUSH(pass);
        break;
    case RPOP:
        status = writeRPOP(pass);
        break;
    default:
        return false;
    }
    return status;
}

/* 機械語命令の書込 */
/* 書込に成功した場合はtrue、それ以外の場合はfalseを返す */
bool cometcmd(const CMDLINE *cmdl, PASS pass)
{
    WORD cmd, adr, r1, r2, x;
    bool status = false;

    /* オペランドなし */
    if(cmdl->opd->opdc == 0) {
        if((cmd = getcmdcode(cmdl->cmd, NONE)) == 0xFFFF) {
            setcerr(112, cmdl->cmd);    /* not command of no operand */
            return false;
        }
        if(writememory(cmd, ptr++, pass) == true) {
            status = true;
        }
    }
    /* 第1オペランドは汎用レジスタ */
    else if((r1 = getgr(cmdl->opd->opdv[0], false)) != 0xFFFF) {
        /* オペランド数1 */
        if(cmdl->opd->opdc == 1) {
            if((cmd = getcmdcode(cmdl->cmd, R_)) == 0xFFFF) {
                setcerr(108, cmdl->cmd);    /* not command of operand "r" */
                return false;
            }
            cmd |= (r1 << 4);
            if(writememory(cmd, ptr++, pass) == true) {
                status = true;
            }
        }
        /* オペランド数2。第2オペランドは汎用レジスタ */
        else if(cmdl->opd->opdc == 2 && (r2 = getgr(cmdl->opd->opdv[1], false)) != 0xFFFF) {
            if((cmd = getcmdcode(cmdl->cmd, R1_R2)) == 0xFFFF) {
                setcerr(109, cmdl->cmd);    /* not command of operand "r1,r2" */
                return false;
            }
            cmd |= ((r1 << 4) | r2);
            if(cerrno == 0 && writememory(cmd, ptr++, pass) == true) {
                status = true;
            }
        }
        /* オペランド数2〜3。第2オペランドはアドレス、
           第3オペランドは指標レジスタとして用いる汎用レジスタ */
        else if(cmdl->opd->opdc == 2 || cmdl->opd->opdc == 3) {
            if((cmd = getcmdcode(cmdl->cmd, R_ADR_X_)) == 0xFFFF &&
               (cmd = getcmdcode(cmdl->cmd, R_ADR_X)) == 0xFFFF)
            {
                setcerr(110, cmdl->cmd);    /* not command of operand "r,adr[,x]" */
                return false;
            }
            cmd |= (r1 << 4);
            /* オペランド数3 */
            if(cmdl->opd->opdc == 3) {
                if((x = getgr(cmdl->opd->opdv[2], true)) == 0xFFFF) {
                    setcerr(125, cmdl->cmd);    /* not GR in operand x */
                    return false;
                }
                cmd |= x;
            }
            adr = getadr(prog, cmdl->opd->opdv[1], pass);
            writememory(cmd, ptr++, pass);
            writememory(adr, ptr++, pass);
            if(cerrno == 0) {
                status = true;
            }
        } else {
            setcerr(113, cmdl->cmd);    /* command not defined */
            return false;
        }
    }
    /* オペランド数1〜2。第1オペランドはアドレス */
    else if(cmdl->opd->opdc == 1 || cmdl->opd->opdc == 2) {
        if((cmd = getcmdcode(cmdl->cmd, ADR_X)) == 0xFFFF) {
            setcerr(111, cmdl->cmd);    /* not command of operand "adr[,x]" */
            return false;
        }
        /* オペランド数2の場合、第2オペランドは指標レジスタとして用いる汎用レジスタ */
        if(cmdl->opd->opdc == 2) {
            x = getgr(cmdl->opd->opdv[1], true);
            if(cerrno > 0) {
                return false;
            }
            cmd |= x;
        }
        /* CALLの場合はプログラムの入口名を表すラベルを取得 */
        /* CALL以外の命令の場合と、プログラムの入口名を取得できない場合は、 */
        /* 同一プログラム内のラベルを取得 */
        if(pass == SECOND && cmd == 0x8000) {        /* CALL命令 */
            adr = getlabel(NULL, cmdl->opd->opdv[0]);
        }
        if(cmd != 0x8000 || (pass == SECOND && adr == 0xFFFF)) {
            adr = getadr(prog, cmdl->opd->opdv[0], pass);
        }
        writememory(cmd, ptr++, pass);
        writememory(adr, ptr++, pass);
        if(cerrno == 0) {
            status = true;
        }
    }
    return status;
}

/* 命令行を1行アセンブルする */
bool assembleline(const CMDLINE *cmdl, PASS pass)
{
    bool status = false;
    /* 命令がない場合 */
    if(cmdl->cmd == NULL){
        /* ラベルが定義されていて命令がない場合はエラー */
        if(cmdl->label != NULL) {
            setcerr(105, NULL);    /* no command in the line */
        }
    }
    /* アセンブラ命令の処理 */
    else if(cerrno == 0 && assemblecmd(cmdl, pass) == true) {
        ;
    }
    /* マクロ命令の書込 */
    else if(cerrno == 0 && macrocmd(cmdl, pass) == true) {
        ;
    }
    /* 機械語命令の書込 */
    else if(cerrno == 0 && cometcmd(cmdl, pass) == true) {
        ;
    }
    else if(cerrno == 0) {
        setcerr(113, cmdl->cmd);    /* command not defined */
    }
    /* エラーが発生していないか確認 */
    if(cerrno == 0) {
        status = true;
    }
    return status;
}

void printline(FILE *stream, const char *filename, int lineno, char *line) {
    fprintf(stream, "%s:%5d:%s", filename, lineno, line);
}

/* 指定された名前のファイルをアセンブル */
/* 2回実行される */
bool assemble(const char *file, PASS pass)
{
    int lineno = 0;
    bool status = true;
    CMDLINE *cmdl;
    char *line;
    FILE *fp;

    if(create_cmdtype_code() == false) {
        return false;
    }
    if((fp = fopen(file, "r")) == NULL) {
        perror(file);
        return false;
    }
    for(; ;) {
        cmdl = malloc(sizeof(CMDLINE));
        line = malloc(LINESIZE+1);
        if((line = fgets(line, LINESIZE, fp)) == NULL) {
            break;
        }
        lineno++;
        if((pass == FIRST && (&asmode)->src == true) ||
           (pass == SECOND && (&asmode)->asdetail == true))
        {
            printline(stdout, file, lineno, line);
        }
        if((cmdl = linetok(line)) != NULL) {
            if(pass == FIRST && cmdl->label != NULL) {
                if(addlabel(prog, cmdl->label, ptr) == false) {
                    break;
                }
            }
            if(assembleline(cmdl, pass) == false) {
                break;
            }
        }
        if(cerrno > 0) {
            break;
        }
    }
    if(cerrno > 0) {
        fprintf(stderr, "Assemble error - %d: %s\n", cerrno, cerrmsg);
        printline(stderr, file, lineno, line);
        status = false;
    }
    fclose(fp);
    return status;
}
