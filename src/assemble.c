#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "assemble.h"
#include "cerr.h"

/**
 * アセンブルモード: src, label, onlylabel, asdetail, onlyassemble
 */
ASMODE asmode = {false, false, false, false, false};

/**
 * アセンブル時の、現在およびリテラルのアドレスとプログラム入口名: ptr, lptr, prog
 */
ASPTR *asptr;

/**
 * アセンブルのエラー定義
 */
static CERR cerr_assemble[] = {
    { 106, "operand mismatch in assemble command" },
    { 107, "no label in START" },
    { 108, "not command of operand \"r\"" },
    { 109, "not command of operand \"r1,r2\"" },
    { 110, "not command of operand \"r,adr[,x]\"" },
    { 111, "not command of operand \"adr[,x]\"" },
    { 112, "not command of no operand" },
    { 113, "operand too many in COMET II command" },
    { 119, "out of COMET II memory" },
    { 120, "GR0 in operand x" },
    { 122, "cannot create hash table" },
    { 124, "more than one character in literal" },
    { 125, "not GR in operand x" },
};

WORD getadr(const char *prog, const char *str, PASS pass);

WORD getgr(const char *str, bool is_x);

WORD getliteral(const char *str, PASS pass);

bool assemblecmd(const CMDLINE *cmdl, PASS pass);

bool macrocmd(const CMDLINE *cmdl, PASS pass);

void writeIN(const char *ibuf, const char *len, PASS pass);

void writeOUT(const char *obuf, const char *len, PASS pass);

void writeRPUSH(PASS pass);

void writeRPOP(PASS pass);

bool cometcmd(const CMDLINE *cmdl, PASS pass);

void writememory(WORD word, WORD adr, PASS pass);

void writestr(const char *str, bool literal, PASS pass);

void writeDC(const char *str, PASS pass);

bool assembletok(const CMDLINE *cmdl, PASS pass);

bool assembleline(const char *line, PASS pass);

void printline(FILE *stream, const char *filename, int lineno, char *line);

/**
 * 汎用レジスタを表す文字列「GR[0-7]」から、レジスタ番号[0-7]をWORD値で返す
 * 文字列が汎用レジスタを表さない場合は、0xFFFFを返す
 * is_xがtrueの場合は指標レジスタ。GR0が指定された場合は、COMET IIの仕様によりエラー発生
 */
WORD getgr(const char *str, bool is_x)
{
    assert(str != NULL);
    WORD r;

    /* 「GR[0-7]」以外の文字列では、0xFFFFを返して終了 */
    if(!(strlen(str) == 3 && strncmp(str, "GR", 2) == 0 &&
         (*(str+2) >= '0' && *(str+2) <= '0' + (GRSIZE - 1))))
    {
        return 0xFFFF;
    }
    r = (WORD)(*(str+2) - '0');
    /* GR0は指標レジスタとして用いることができない */
    if(is_x == true && r == 0x0) {
        setcerr(120, NULL);    /* GR0 in operand x */
        return 0x0;
    }
    return r;
}

/**
 * 定数の前に等号（=）をつけて記述されるリテラルを返す
 * リテラルには、10進定数／16進定数／文字定数が含まれる
 */
WORD getliteral(const char *str, PASS pass)
{
    assert(*str == '=');
    WORD adr = asptr->lptr;

    if(*(++str) == '\'') {    /* 文字定数 */
        writestr(str, true, pass);
    } else {
        writememory(nh2word(str), (asptr->lptr)++, pass);
    }
    return adr;
}

/**
 * アセンブラ命令をメモリに書込
 * アセンブラ命令の場合はtrue、それ以外の場合はfalseを返す
 */
bool assemblecmd(const CMDLINE *cmdl, PASS pass)
{
    int i = 0;
    ASCMDID cmdid = 0;
    ASCMD ascmd[] = {
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
                return true;
            }
            cmdid = ascmd[i].cmdid;
            break;
        }
    } while(ascmd[++i].cmdid != 0);
    /* アセンブラ命令 */
    switch(cmdid)
    {
    case START:
        if(cmdl->label == NULL) {
            setcerr(107, NULL);    /* no label in START */
            return true;
        }
        /* プログラム名の設定 */
        asptr->prog = strdup_chk(cmdl->label, "asptr.prog");
        /* オペランドがある場合、実行開始アドレスを設定 */
        if(pass == SECOND && cmdl->opd->opdc == 1) {
            if((execptr->start = getlabel(asptr->prog, cmdl->opd->opdv[0])) == 0xFFFF) {
                setcerr(103, cmdl->opd->opdv[0]);    /* label not found */
            }
        }
        break;
    case END:
        /* 1回目のアセンブルの場合は、リテラル領域開始アドレスを設定 */
        if(pass == FIRST) {
            asptr->lptr = asptr->ptr;
        }
        /* 2回目のアセンブルの場合は、リテラル領域終了アドレスを実行終了アドレスとして設定 */
        else if(pass == SECOND) {
            execptr->end = asptr->lptr;
        }
        FREE(asptr->prog);
        break;
    case DS:
        for(i = 0; i < atoi(cmdl->opd->opdv[0]); i++) {
            writememory(0x0, (asptr->ptr)++, pass);
            if(cerr->num > 0) {
                break;
            }
        }
        break;
    case DC:
        for(i = 0; i < cmdl->opd->opdc; i++) {
            writeDC(cmdl->opd->opdv[i], pass);
            if(cerr->num > 0) {
                break;
            }
        }
        break;
    default:
        return false;
    }
    return true;
}

/**
 *  macrocmd
 * マクロ命令をメモリに書込
 * マクロ命令の場合はtrue、それ以外の場合はfalseを返す
 */
bool macrocmd(const CMDLINE *cmdl, PASS pass)
{
    int i = 0;
    MACROCMDID cmdid = 0;
    MACROCMD macrocmd[] = {
        { IN, 2, 2, "IN" },
        { OUT, 2, 2, "OUT" },
        { RPUSH, 0, 0, "RPUSH" },
        { RPOP, 0, 0, "RPOP" },
        { 0, 0, 0, NULL }
    };

    do {
        if(strcmp(cmdl->cmd, macrocmd[i].cmd) == 0) {
            if(cmdl->opd->opdc < macrocmd[i].opdc_min ||
               cmdl->opd->opdc > macrocmd[i].opdc_max)
            {
                setcerr(106, NULL);    /* operand count mismatch */
                return true;
            }
            cmdid = macrocmd[i].cmdid;
            break;
        }
    } while(macrocmd[++i].cmdid != 0);
    switch(cmdid)
    {
    case IN:
        writeIN(cmdl->opd->opdv[0], cmdl->opd->opdv[1], pass);
        break;
    case OUT:
        writeOUT(cmdl->opd->opdv[0], cmdl->opd->opdv[1], pass);
        break;
    case RPUSH:
        writeRPUSH(pass);
        break;
    case RPOP:
        writeRPOP(pass);
        break;
    default:
        return false;
    }
    return true;
}

/**
 * マクロ命令「IN IBUF,LEN」をメモリに書込
 *      PUSH 0,GR1
 *      PUSH 0,GR2
 *      LAD GR1,IBUF
 *      LAD GR2,LEN
 *      SVC 1
 *      POP GR2
 *      POP GR1
 */
void writeIN(const char *ibuf, const char *len, PASS pass)
{
    char *line = malloc_chk(LINESIZE + 1, "writeIN.line");

    assembleline("    PUSH 0,GR1", pass);
    assembleline("    PUSH 0,GR2", pass);
    sprintf(line, "    LAD GR1,%s", ibuf);
    assembleline(line, pass);
    sprintf(line, "    LAD GR2,%s", len);
    assembleline(line, pass);
    assembleline("    SVC 1", pass);
    assembleline("    POP GR2", pass);
    assembleline("    POP GR1", pass);

    FREE(line);
}

/**
 *  マクロ命令「OUT OBUF,LEN」をメモリに書込
 *      PUSH 0,GR1
 *      PUSH 0,GR2
 *      LAD GR1,OBUF
 *      LAD GR2,LEN
 *      SVC 2
 *      LAD GR1,=#A
 *      LAD GR2,=1
 *      SVC 2
 *      POP GR2
 *      POP GR1
 */
void writeOUT(const char *obuf, const char *len, PASS pass)
{
    char *line = malloc_chk(LINESIZE + 1, "writeOUT.line");

    assembleline("    PUSH 0,GR1", pass);
    assembleline("    PUSH 0,GR2", pass);
    sprintf(line, "    LAD GR1,%s", obuf);
    assembleline(line, pass);
    sprintf(line, "    LAD GR2,%s", len);
    assembleline(line, pass);
    assembleline("    SVC 2", pass);
    assembleline("    LAD GR1,=#A", pass);
    assembleline("    LAD GR2,=1", pass);
    assembleline("    SVC 2", pass);
    assembleline("    POP GR2", pass);
    assembleline("    POP GR1", pass);
    FREE(line);
}

/** マクロ命令「RPUSH」をメモリに書き込む
 *       PUSH 0,GR1
 *       PUSH 0,GR2
 *       PUSH 0,GR3
 *       PUSH 0,GR4
 *       PUSH 0,GR5
 *       PUSH 0,GR6
 *       PUSH 0,GR7
 */
void writeRPUSH(PASS pass)
{
    int i;
    char *line = malloc_chk(LINESIZE + 1, "writeRPUSH.line");

    for(i = 1; i <= GRSIZE-1; i++) {
        sprintf(line, "    PUSH 0,GR%d", i);
        assembleline(line, pass);
    }
    FREE(line);
}

/**
 * マクロ命令「RPOP」をメモリに書き込む
 *      POP GR7
 *      POP GR6
 *      POP GR5
 *      POP GR4
 *      POP GR3
 *      POP GR3
 *      POP GR2
 *      POP GR1
 */
void writeRPOP(PASS pass)
{
    int i;
    char *line = malloc_chk(LINESIZE + 1, "writeRPOP.line");

    for(i = GRSIZE-1; i >= 1; i--) {
        sprintf(line, "    POP GR%d", i);
        assembleline(line, pass);
    }
    FREE(line);
}

/**
 * 機械語命令をメモリに書込
 * 書込に、成功した場合はtrue、失敗した場合はfalse、を返す
 */
bool cometcmd(const CMDLINE *cmdl, PASS pass)
{
    WORD cmd, r_r1, x_r2, adr;

    /* オペランドなし */
    if(cmdl->opd->opdc == 0) {
        if((cmd = getcmdcode(cmdl->cmd, NONE)) == 0xFFFF) {
            setcerr(112, cmdl->cmd);    /* not command of no operand */
            return false;
        }
        writememory(cmd, (asptr->ptr)++, pass);
    }
    /* 第1オペランドは汎用レジスタ */
    else if((r_r1 = getgr(cmdl->opd->opdv[0], false)) != 0xFFFF) {
        /* オペランド数1 */
        if(cmdl->opd->opdc == 1) {
            if((cmd = getcmdcode(cmdl->cmd, R_)) == 0xFFFF) {
                setcerr(108, cmdl->cmd);    /* not command of operand "r" */
                return false;
            }
            cmd |= (r_r1 << 4);
            writememory(cmd, (asptr->ptr)++, pass);
        }
        /* オペランド数2。第2オペランドは汎用レジスタ */
        else if(cmdl->opd->opdc == 2 && (x_r2 = getgr(cmdl->opd->opdv[1], false)) != 0xFFFF) {
            if((cmd = getcmdcode(cmdl->cmd, R1_R2)) == 0xFFFF) {
                setcerr(109, cmdl->cmd);    /* not command of operand "r1,r2" */
                return false;
            }
            cmd |= ((r_r1 << 4) | x_r2);               /* 第1オペランド、第2オペランドともに汎用レジスタ */
            /* メモリへの書き込み */
            writememory(cmd, (asptr->ptr)++, pass);
        }
        /* オペランド数2または3 */
        else if(cmdl->opd->opdc == 2 || cmdl->opd->opdc == 3) {
            if((cmd = getcmdcode(cmdl->cmd, R_ADR_X)) == 0xFFFF) {
                setcerr(110, cmdl->cmd);    /* not command of operand "r,adr[,x]" */
                return false;
            }
            cmd |= (r_r1 << 4);                    /* 第1オペランドは汎用レジスタ */
            /* オペランド数3の場合 */
            if(cmdl->opd->opdc == 3) {             /* 第3オペランドは指標レジスタとして用いる汎用レジスタ */
                if((x_r2 = getgr(cmdl->opd->opdv[2], true)) == 0xFFFF) {
                    setcerr(125, cmdl->cmd);    /* not GR in operand x */
                    return false;
                }
                cmd |= x_r2;
            }
            adr = getadr(asptr->prog, cmdl->opd->opdv[1], pass); /* 第2オペランドはアドレス */
            /* メモリへの書き込み */
            writememory(cmd, (asptr->ptr)++, pass);
            writememory(adr, (asptr->ptr)++, pass);
        } else {
            setcerr(113, cmdl->cmd);    /* operand too many in COMET II command */
            return false;
        }
    }
    /* オペランド数1または2。第1オペランドはアドレス */
    else if(cmdl->opd->opdc == 1 || cmdl->opd->opdc == 2) {
        if((cmd = getcmdcode(cmdl->cmd, ADR_X)) == 0xFFFF) {
            setcerr(111, cmdl->cmd);    /* not command of operand "adr[,x]" */
            return false;
        }
        /* オペランド数2の場合 */
        if(cmdl->opd->opdc == 2) {             /* 第2オペランドは指標レジスタとして用いる汎用レジスタ */
            x_r2 = getgr(cmdl->opd->opdv[1], true);
            if(cerr->num > 0) {
                return false;
            }
            cmd |= x_r2;
        }
        /* CALLの場合はプログラムの入口名を表すラベルを取得 */
        /* CALL以外の命令の場合と、プログラムの入口名を取得できない場合は、 */
        /* 同一プログラム内のラベルを取得 */
        if(pass == SECOND && cmd == 0x8000) {        /* CALL命令 */
            adr = getlabel(NULL, cmdl->opd->opdv[0]);
        }
        if(cmd != 0x8000 || (pass == SECOND && adr == 0xFFFF)) {
            adr = getadr(asptr->prog, cmdl->opd->opdv[0], pass);
        }
        /* メモリへの書き込み */
        writememory(cmd, (asptr->ptr)++, pass);
        writememory(adr, (asptr->ptr)++, pass);
    }
    return (cerr->num == 0) ? true : false;
}

/**
 * COMET IIのメモリにアドレス値を書き込む
 */
void writememory(WORD word, WORD adr, PASS pass)
{
    /* COMET IIメモリオーバーの場合、エラー発生 */
    if(adr >= sys->memsize) {
        setcerr(119, word2n(adr));    /* out of COMET II memory */
        return;
    }
    (sys->memory)[adr] = word;
    if(pass == SECOND && asmode.asdetail == true) {
        fprintf(stdout, "\t#%04X\t#%04X\n", adr, word);
    }
}

/**
 * 文字をメモリに書き込む
 */
void writestr(const char *str, bool literal, PASS pass)
{
    assert(*str == '\'');
    const char *p = str + 1;
    bool lw = false;

    for(; ;) {
        /* 閉じ「'」がないまま文字列が終了した場合 */
        if(*p == '\0') {
            setcerr(123, str);    /* unclosed quote */
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
            writememory(*(p++), (asptr->lptr)++, pass);
            lw = true;
        } else {
            writememory(*(p++), (asptr->ptr)++, pass);
        }
    }
}

/**
 * DC命令の内容を書き込む
 */
void writeDC(const char *str, PASS pass)
{
    WORD adr = 0x0;

    if(*str == '\'') {
        writestr(str, false, pass);
    } else {
        if(*str == '#' || isdigit(*str) || *str == '-') {
            adr = nh2word(str);
        } else {
            if(pass == SECOND && (adr = getlabel(asptr->prog, str)) == 0xFFFF) {
                setcerr(103, str);    /* label not found */
            }
        }
        writememory(adr, (asptr->ptr)++, pass);
    }
}

/**
 * トークンをアセンブル
 */
bool assembletok(const CMDLINE *cmdl, PASS pass)
{
    /* 命令がない場合 */
    if(cmdl->cmd == NULL){
        return true;
    }
    /* アセンブラ命令またはマクロ命令の書込 */
    if(assemblecmd(cmdl, pass) == false && macrocmd(cmdl, pass) == false) {
        /* COMET II命令の書込 */
        if(cometcmd(cmdl, pass) == false && cerr->num == 0) {
            setcerr(113, cmdl->cmd);    /* operand too many in COMET II command */
        }
    }
    return (cerr->num == 0) ? true : false;
}

/**
 * ファイルストリームの現在行を番号付きで表示する
 */
void printline(FILE *stream, const char *filename, int lineno, char *line)
{
    fprintf(stream, "%s:%5d:%s", filename, lineno, line);
}

/**
 * アドレスを返す
 * アドレスには、リテラル／10進定数／16進定数／アドレス定数が含まれる
 */
WORD getadr(const char *prog, const char *str, PASS pass)
{
    WORD adr = 0x0;

    if(*str == '=') {
        adr = getliteral(str, pass);
    } else if(isdigit(*str) || *str == '-' || *str == '#') {
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


/**
 * 1行をアセンブル
 * passが1の場合はラベルを登録し、2の場合はラベルからアドレスを読み込む
 */
bool assembleline(const char *line, PASS pass)
{
    CMDLINE *cmdl;
    bool stat = true;
    int i;

    cmdl = linetok(line);
    stat = (cerr->num == 0) ? true : false;
    if(cmdl != NULL) {
        if(stat == true) {
            if(pass == FIRST && cmdl->label != NULL) {
                stat = addlabel(asptr->prog, cmdl->label, asptr->ptr);
            }
            if(stat == true) {
                stat = assembletok(cmdl, pass);
            }
            FREE(cmdl->label);
        }
        if(cmdl->opd != NULL) {
            for(i = 0; i < cmdl->opd->opdc; i++) {
                FREE(cmdl->opd->opdv[i]);
            }
        }
        FREE(cmdl->opd);
        FREE(cmdl->cmd);
    }
    FREE(cmdl);
    return stat;
}

/**
 * アセンブルのエラーをエラーリストに追加
 */
void addcerrlist_assemble()
{
    addcerrlist_tok();
    addcerrlist_word();
    addcerrlist_label();
    addcerrlist(ARRAYSIZE(cerr_assemble), cerr_assemble);
}

/**
 * 指定された名前のファイルをアセンブル
 * アセンブル完了時はtrue、エラー発生時はfalseを返す
 */
bool assemblefile(const char *file, PASS pass)
{
    int lineno = 1;
    char *line;
    FILE *fp;

    if((fp = fopen(file, "r")) == NULL) {
        perror(file);
        return false;
    }
    for(line = malloc_chk(LINESIZE + 1, "assemble.line"); fgets(line, LINESIZE, fp); lineno++) {
        if((pass == FIRST && asmode.src == true) || (pass == SECOND && asmode.asdetail == true)) {
            printline(stdout, file, lineno, line);
        }
        if(assembleline(line, pass) == false) {
            break;
        }
    }
    if(cerr->num > 0) {
        fprintf(stderr, "Assemble error - %d: %s\n", cerr->num, cerr->msg);
        printline(stderr, file, lineno, line);
    }
    FREE(line);
    fclose(fp);
    return (cerr->num == 0) ? true : false;
}

/**
 * 引数で指定したファイルにアセンブル結果を書込
 */
void outassemble(const char *file)
{
    FILE *fp;

    if((fp = fopen(file, "w")) == NULL) {
        perror(file);
        exit(-1);
    }
    fwrite(sys->memory, sizeof(WORD), execptr->end, fp);
    fclose(fp);
}
