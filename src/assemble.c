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
 * アセンブルのプロパティ: ptr, lptr, *prog
 */
ASPROP *asprop;

/**
 * アセンブルのエラー定義
 */
static CERR cerr_assemble[] = {
    { 101, "label already defined" },
    { 102, "label table is full" },
    { 103, "label not found" },
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

bool writememory(WORD word, WORD adr, PASS pass);

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
    WORD adr = asprop->lptr;
    assert(*str == '=');
    if(*(++str) == '\'') {    /* 文字定数 */
        writestr(str, true, pass);
    } else {
        writememory(nh2word(str), (asprop->lptr)++, pass);
    }
    return adr;
}

/**
 * アセンブラ命令をメモリに書込
 * 実行に成功した場合はtrue、それ以外の場合はfalseを返す
 */
bool assemblecmd(const CMDLINE *cmdl, PASS pass)
{
    int i = 0;
    ASCMDID cmdid = 0;
    bool status = false;
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
                return false;
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
            return false;
        }
        /* プログラム名の設定 */
        asprop->prog = strdup_chk(cmdl->label, "asprop.prog");
        /* オペランドがある場合、実行開始番地を設定 */
        if(pass == SECOND && cmdl->opd->opdc == 1) {
            if((prog->start = getlabel(asprop->prog, cmdl->opd->opdv[0])) == 0xFFFF) {
                setcerr(103, cmdl->opd->opdv[0]);    /* label not found */
            }
        }
        status = true;
        break;
    case END:
        /* 1回目のアセンブルの場合は、リテラル領域開始番地を設定 */
        if(pass == FIRST) {
            asprop->lptr = asprop->ptr;
        }
        /* 2回目のアセンブルの場合は、リテラル領域終了番地を実行終了番地として設定 */
        else if(pass == SECOND) {
            prog->end = asprop->lptr;
        }
        /* プログラム名のクリア */
        asprop->prog = NULL;
        status = true;
        break;
    case DS:
        for(i = 0; i < atoi(cmdl->opd->opdv[0]); i++) {
            writememory(0x0, (asprop->ptr)++, pass);
            if(cerr->num > 0) {
                return false;
            }
        }
        status = true;
        break;
    case DC:
        for(i = 0; i < cmdl->opd->opdc; i++) {
            writeDC(cmdl->opd->opdv[i], pass);
            if(cerr->num > 0) {
                return false;
            }
        }
        status = true;
        break;
    default:
        return false;
    }
    if(cerr->num > 0) {
        status = false;
    }
    return status;
}

/**
 *  macrocmd
 *  マクロ命令をメモリに書込
 *  書込に成功した場合はtrue、それ以外の場合はfalseを返す
 */
bool macrocmd(const CMDLINE *cmdl, PASS pass)
{
    int i = 0;
    MACROCMDID cmdid;
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
                return false;
            }
            cmdid = macrocmd[i].cmdid;
            break;
        }
    } while(macrocmd[++i].cmdid != 0);
    switch(cmdid)
    {
    case IN:
        writeIN(cmdl->opd->opdv[0], cmdl->opd->opdv[1], pass);
        return true;
    case OUT:
        writeOUT(cmdl->opd->opdv[0], cmdl->opd->opdv[1], pass);
        return true;
    case RPUSH:
        writeRPUSH(pass);
        return true;
    case RPOP:
        writeRPOP(pass);
        return true;
    default:
        return false;
    }
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
    char *line = malloc_chk(LINESIZE+1, "writeIN.line");

    assembleline("    PUSH 0,GR1", pass);
    assembleline("    PUSH 0,GR2", pass);
    sprintf(line, "    LAD GR1,%s", ibuf);
    assembleline(line, pass);
    sprintf(line, "    LAD GR2,%s", len);
    assembleline(line, pass);
    assembleline("    SVC 1", pass);
    assembleline("    POP GR2", pass);
    assembleline("    POP GR1", pass);

    free_chk(line, "writeIN.line");
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
    char *line = malloc_chk(LINESIZE+1, "writeOUT.line");

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
    free_chk(line, "writeOUT.line");
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
    char *line = malloc_chk(LINESIZE+1, "writeRPUSH.line");

    for(i = 1; i <= GRSIZE-1; i++) {
        sprintf(line, "    PUSH 0,GR%d", i);
        assembleline(line, pass);
    }
    free_chk(line, "writeRPUSH.line");
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
    char *line = malloc_chk(LINESIZE+1, "writeRPOP.line");

    for(i = GRSIZE-1; i >= 1; i--) {
        sprintf(line, "    POP GR%d", i);
        assembleline(line, pass);
    }
    free_chk(line, "writeRPOP.line");
}

/**
 * 機械語命令をメモリに書込
 * 書込に、成功した場合はtrue、失敗した場合はfalse、を返す
 */
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
        if(writememory(cmd, (asprop->ptr)++, pass) == true) {
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
            if(writememory(cmd, (asprop->ptr)++, pass) == true) {
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
            if(cerr->num == 0 && writememory(cmd, (asprop->ptr)++, pass) == true) {
                status = true;
            }
        }
        /* オペランド数2または3。第2オペランドはアドレス、 */
        /* 第3オペランドは指標レジスタとして用いる汎用レジスタ */
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
            adr = getadr(asprop->prog, cmdl->opd->opdv[1], pass);
            writememory(cmd, (asprop->ptr)++, pass);
            writememory(adr, (asprop->ptr)++, pass);
            if(cerr->num == 0) {
                status = true;
            }
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
        /* オペランド数2の場合、第2オペランドは指標レジスタとして用いる汎用レジスタ */
        if(cmdl->opd->opdc == 2) {
            x = getgr(cmdl->opd->opdv[1], true);
            if(cerr->num > 0) {
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
            adr = getadr(asprop->prog, cmdl->opd->opdv[0], pass);
        }
        writememory(cmd, (asprop->ptr)++, pass);
        writememory(adr, (asprop->ptr)++, pass);
        if(cerr->num == 0) {
            status = true;
        }
    }
    return status;
}

/**
 * COMET IIのメモリにアドレス値を書き込む
 */
bool writememory(WORD word, WORD adr, PASS pass)
{
    bool status = false;

    /* COMET IIメモリオーバーの場合 */
    if(adr >= sys->memsize) {
        setcerr(119, word2n(adr));    /* out of COMET II memory */
    }
    if(cerr->num == 0) {
        (sys->memory)[adr] = word;
        if(pass == SECOND && asmode.asdetail == true) {
            fprintf(stdout, "\t#%04X\t#%04X\n", adr, word);
        }
        status = true;
    }
    return status;
}

/**
 * 文字をメモリに書き込む
 */
void writestr(const char *str, bool literal, PASS pass)
{
    assert(cerr->num == 0 && *str == '\'');
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
            writememory(*(p++), (asprop->lptr)++, pass);
            lw = true;
        } else {
            writememory(*(p++), (asprop->ptr)++, pass);
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
            if(pass == SECOND && (adr = getlabel(asprop->prog, str)) == 0xFFFF) {
                setcerr(103, str);    /* label not found */
            }
        }
        writememory(adr, (asprop->ptr)++, pass);
    }
}

/**
 * 1行をアセンブル
 */
bool assembletok(const CMDLINE *cmdl, PASS pass)
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
    else if(cerr->num == 0 && assemblecmd(cmdl, pass) == true) {
        ;
    }
    /* マクロ命令の書込 */
    else if(cerr->num == 0 && macrocmd(cmdl, pass) == true) {
        ;
    }
    /* 機械語命令の書込 */
    else if(cerr->num == 0 && cometcmd(cmdl, pass) == true) {
        ;
    }
    else if(cerr->num == 0) {
        setcerr(113, cmdl->cmd);    /* operand too many in COMET II command */
    }
    /* エラーが発生していないか確認 */
    if(cerr->num == 0) {
        status = true;
    }
    return status;
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
 */
bool assembleline(const char *line, PASS pass)
{
    CMDLINE *cmdl;

    if((cmdl = linetok(line)) != NULL) {
        if(pass == FIRST && cmdl->label != NULL) {
            if(addlabel(asprop->prog, cmdl->label, asprop->ptr) == false) {
                return false;
            }
        }
        if(assembletok(cmdl, pass) == false) {
            return false;
        }
    }
    return true;
}

/**
 * アセンブルのエラーをエラーリストに追加
 */
void addcerrlist_assemble()
{
    addcerrlist_tok();
    addcerrlist_word();
    addcerrlist(ARRAYSIZE(cerr_assemble), cerr_assemble);
}

/**
 * 指定された名前のファイルをアセンブル
 * 2回実行される
 */
bool assemble(const char *file, PASS pass)
{
    int lineno = 0;
    bool status = true;
    CMDLINE *cmdl;
    char *line;
    FILE *fp;

    if((fp = fopen(file, "r")) == NULL) {
        perror(file);
        return false;
    }
    for(; ;) {
        cmdl = malloc_chk(sizeof(CMDLINE), "cmdl");
        line = malloc_chk(LINESIZE + 1, "line");
        if((line = fgets(line, LINESIZE, fp)) == NULL) {
            break;
        }
        lineno++;
        if((pass == FIRST && asmode.src == true) ||
           (pass == SECOND && asmode.asdetail == true))
        {
            printline(stdout, file, lineno, line);
        }
        if(assembleline(line, pass) == false) {
            break;
        }
        if(cerr->num > 0) {
            break;
        }
        free_chk(line, "line");
        free_chk(cmdl, "cmdl");
    }
    if(cerr->num > 0) {
        fprintf(stderr, "Assemble error - %d: %s\n", cerr->num, cerr->msg);
        printline(stderr, file, lineno, line);
        status = false;
    }
    fclose(fp);
    return status;
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
    fwrite(sys->memory, sizeof(WORD), prog->end, fp);
    fclose(fp);
}
