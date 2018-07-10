#include "monitor.h"

/**
 * @brief ブレークポイント表
 */
static BPSLIST *bps[BPSTABSIZE];

unsigned adrhash(WORD adr)
{
    HKEY *key[1];
    unsigned h;
    key[0] = malloc_chk(sizeof(HKEY), "adrhash.key");
    key[0]->type = INT;
    key[0]->val.i = adr;
    h = hash(1, key, BPSTABSIZE);
    FREE(key[0]);
    return h;
}

bool getbps(WORD adr)
{
    BPSLIST *p;

    for(p = bps[adrhash(adr)]; p != NULL; p = p->next) {
        if(p->adr == adr) {
            return true;
        }
    }
    return false;
}

bool addbps(WORD adr)
{
    BPSLIST *p;
    unsigned h;

    /* 登録されたラベルを検索。すでに登録されている場合は終了 */
    if(getbps(adr) == true) {
        fprintf(stderr, "%04X: Breakpoint is already defined.\n", adr);
        return false;
    }
    /* メモリを確保 */
    p = malloc_chk(sizeof(BPSLIST), "bps.next");
    /* アドレスを設定 */
    p->adr = adr;
    /* ハッシュ表へ追加 */
    p->next = bps[h = adrhash(adr)];
    bps[h] = p;
    return true;
}

bool delbps(WORD adr)
{
    BPSLIST *p, *q;
    unsigned h;
    bool res = false;

    p = bps[h = adrhash(adr)];
    if(p != NULL) {
        if(p->adr == adr) {
            if(p->next == NULL) {
                FREE(bps[h]);
            } else {
                bps[h] = p->next;
                FREE(p);
            }
            res = true;
        } else {
            for(; p->next != NULL; p = p->next) {
                q = p->next;
                if(q->adr == adr) {
                    p->next = q->next;
                    FREE(q);
                    res = true;
                    break;
                }
            }
        }
    }
    return res;
}

void listbps()
{
    int i, cnt = 0;
    BPSLIST *p;

    fprintf(stdout, "List of breakpoints\n");
    for(i = 0; i < BPSTABSIZE; i++) {
        for(p = bps[i]; p != NULL; p = p->next) {
            fprintf(stdout, "#%04X\n", p->adr);
            cnt++;
        }
    }
    if(cnt == 0) {
        fprintf(stdout, "(No breakpoints.)\n");
    }
}

void freebpslist(BPSLIST *head)
{
    BPSLIST *p, *q;
    for(p = head; p != NULL; p = q) {
        q = p->next;
        FREE(p);
    }
}

void freebps()
{
    int i;
    for(i = 0; i < BPSTABSIZE; i++) {
        freebpslist(bps[i]);
        bps[i] = NULL;
    }
}

MONARGS *monargstok(const char *str)
{
    MONARGS *args = malloc_chk(sizeof(MONARGS), "args");
    char *p, *q, *r, *sepp;     /* pは文字列全体の先頭位置、qはトークンの先頭位置、rは文字の位置 */
    int sepc = ' ';

    args->argc = 0;
    if(str == NULL) {
        return args;
    }
    p = q = r = strdup_chk(str, "argstok.p");
    do {
        while(*q == ' ' || *q == '\t'){
            q = ++r;
        }
        sepp = r + strcspn(r, " ");
        sepc = *sepp;
        *sepp = '\0';
        args->argv[++(args->argc)-1] = strdup_chk(q, "args.argv[]");
        q = r = sepp + 1;
    } while(sepc == ' ');
    FREE(p);
    return args;
}

MONCMDLINE *monlinetok(const char *line)
{
    char *tokens, *p;
    long l;
    MONCMDLINE *moncmdl = NULL;

    if(*line == '\n' || *line == '\0') {
        return NULL;
    }
    p = tokens = strdup_chk(line, "tokens");
    moncmdl = malloc_chk(sizeof(MONCMDLINE), "moncmdl");
    /* コマンドの取得 */
    moncmdl->cmd = malloc_chk((l = strcspn(p, " \t\n")) + 1, "moncmdl.cmd");
    strncpy(moncmdl->cmd, p, l);
    /* コマンドと引数の間の空白をスキップ */
    p += l;
    while(*p == ' ' || *p == '\t') {
        p++;
    }
    /* 引数として、改行までの文字列を取得 */
    if((l = strcspn(p, "\n")) > 0) {
        moncmdl->args = monargstok(p);
    } else {
        moncmdl->args = malloc_chk(sizeof(MONARGS), "moncmdl.args");
        moncmdl->args->argc = 0;
    }
    FREE(tokens);
    return moncmdl;
}

bool stracmp(char *str1, int str2c, char *str2v[])
{
    int i;
    if(str1 == NULL) {
        return false;
    }
    for(i = 0; i < str2c; i++) {
        if(strcmp(str1, str2v[i]) == 0) {
            return true;
        }
    }
    return false;
}

void mon_break(int argc, char *argv[])
{
    WORD w;
    if(stracmp(argv[0], 2, (char* []){"l", "list"})) {
        listbps();
    } else if(stracmp(argv[0], 2, (char* []){"r", "reset"})) {
        freebps();
        fprintf(stdout, "All breakpoints are deleted.\n");
    } else {
        if(argc > 1) {
            if((w = nh2word(argv[1])) == 0x0) {
                fprintf(stderr, "%s: address error\n", argv[1]);
            }
        }
        if(stracmp(argv[0], 2, (char* []){"a", "add"})) {
            if(addbps(w) == true) {
                fprintf(stdout, "#%04X: breakpoint added\n", w);
            } else {
                fprintf(stdout, "No breakpoint added\n");
            }
        } else if(stracmp(argv[0], 2, (char* []){"d", "del"})) {
            if(delbps(w) == true) {
                fprintf(stdout, "#%04X: breakpoint deleted\n", w);
            } else {
                fprintf(stdout, "No breakpoint deleted\n");
            }
        } else if(stracmp(argv[0], 3, (char* []){"?", "h", "help"})) {
            fprintf(stdout, "breakpoint manipulate:\n");
            fprintf(stdout, "    b[reak] a[dd] <address>\n");
            fprintf(stdout, "    b[reak] d[el] <address>\n");
            fprintf(stdout, "    b[reak] l[ist]\n");
            fprintf(stdout, "    b[reak] r[eset]\n");
        } else {
            fprintf(stderr, "%s: Not breakpoint manipulate command. see `b ?'.\n", argv[0]);
        }
    }
}

void mon_dump(int argc, char *argv[])
{
    int i = 0;
    WORD start = 0, end = 0xFFFF;
    if(argc > 0 && stracmp(argv[0], 2, (char* []){"a", "auto"})) {
        execmode.dump = true;
    } else if(argc > 0 && stracmp(argv[0], 2, (char* []){"no", "noauto"})) {
        execmode.dump = false;
    } else {
        if(argc > 0) {
            start = nh2word(argv[0]);
            if(argc > 1) {
                end = nh2word(argv[1]);
            }
        }
        dumpmemory(start, end);
        if(argc > 2) {
            for(i = 2; i < argc; i++) {
                if(i > 2) {
                    fprintf(stderr, " ");
                }
                fprintf(stderr, "%s", argv[i+1]);
            }
            fprintf(stderr, ": ignored.\n");
        }
    }
}

MONCMDTYPE monitorcmd(char *cmd, MONARGS *args)
{
    MONCMDTYPE cmdtype = MONREPEAT;
    if(stracmp(cmd, 2, (char* []){"a", "assemble"})) {
        if(args->argc == 0) {
            fprintf(stderr, "Error: Input file name.\n");
        } else if(args->argc == 1) {
            assemble(1, (char* []){args->argv[0]}, 0);
        } else {
            assemble(1, (char* []){args->argv[0]}, nh2word(args->argv[1]));
        }
    } else if(stracmp(cmd, 2, (char* []){"b", "break"})) {
        mon_break(args->argc, args->argv);
    } else if(stracmp(cmd, 2, (char* []){"c", "continue"})) {
        execmode.step = false;
        cmdtype = MONNEXT;
    } else if(stracmp(cmd, 2, (char* []){"d", "dump"})) {
        mon_dump(args->argc, args->argv);
    } else if(stracmp(cmd, 2, (char* []){"l", "load"})) {
        loadassemble(args->argv[0], nh2word(args->argv[1]));
    } else if(stracmp(cmd, 2, (char* []){"n", "next"})) {
        execmode.step = true;
        cmdtype = MONNEXT;
    } else if(stracmp(cmd, 2, (char* []){"q", "quit"})) {
        fprintf(stdout, "Quit: COMET II monitor\n");
        cmdtype = MONQUIT;
    } else if(stracmp(cmd, 2, (char* []){"r", "reverse"})) {
        if(args->argc == 2) {
            disassemble_memory(nh2word(args->argv[0]), nh2word(args->argv[1]));
        }
    } else if(stracmp(cmd, 1, (char* []){"reset"})) {
        fprintf(stdout, "Reset COMET II.\n");
        reset(sys->memsize, sys->clocks);     /* COMET II仮想マシンのリセット */
    } else if(stracmp(cmd, 2, (char* []){"t", "trace"})) {
        if(args->argc > 0 && stracmp(args->argv[0], 2, (char* []){"a", "auto"})) {
            execmode.logical = false;
            execmode.trace = true;
        } else if(args->argc > 0 && stracmp(args->argv[0], 2, (char* []){"no", "noauto"})) {
            execmode.trace = false;
        } else {
            fprintf(stdout, "#%04X: Register::::\n", sys->cpu->pr);
            dspregister();
        }
    } else if(stracmp(cmd, 2, (char* []){"T", "tracelogical"})) {
        if(args->argc > 0 && stracmp(args->argv[0], 2, (char* []){"a", "auto"})) {
            execmode.logical = true;
            execmode.trace = true;
        } else if(args->argc > 0 && stracmp(args->argv[0], 2, (char* []){"no", "noauto"})) {
            execmode.trace = false;
        } else {
            fprintf(stdout, "#%04X: Register::::\n", sys->cpu->pr);
            dspregister();
        }
    } else if(stracmp(cmd, 3, (char* []){"?", "h", "help"})) {
        fprintf(stdout, "b[reak] -- Manipulate Breakpoints. See details, `b ?'.\n");
        fprintf(stdout, "c[ontinue] -- Continue running your program.\n");
        fprintf(stdout, "d[ump] -- Display memory dump. `d[ump] a[uto]/n[oauto]' set auto/noauto display.\n");
        fprintf(stdout, "l[oad] -- Load object from a file to the memory. `l[oad] <filepath> <address>' if address is omitted, load to address 0.\n");
        fprintf(stdout, "n[ext] -- Go next instruction.\n");
        fprintf(stdout, "q[uit] -- Quit running your program.\n");
        fprintf(stdout, "reset -- Reset the system.\n");
        fprintf(stdout, "r[everse] -- Disassemble memory. `r[everse] <start address> <end address>.\n");
        fprintf(stdout, "s[ave] -- Save object from the memory to a file. `s[ave] <filepath> [<start address1> [<end address>]]' if <start address> and <end address> is omitted, save the whole memory. if <end address> is omitted, save the memory after <start address>.\n");
        fprintf(stdout, "t[race] -- Display CPU register. `t[race] a[uto]/n[oauto]' set auto/noauto display. \n");
        fprintf(stdout, "T[race] -- Display CPU register as logical value. `t[race] a[uto]/n[oauto]' set auto/noauto display. \n");
        fprintf(stdout, "?/h[elp] -- Display this help.\n");
    }
    return cmdtype;
}

void free_moncmdline(MONCMDLINE *moncmdl)
{
    int i;
    assert(moncmdl != NULL);
    if(moncmdl->args != NULL) {
        for(i = 0;  i < moncmdl->args->argc; i++) {
            FREE(moncmdl->args->argv[i]);
        }
        FREE(moncmdl->args);
    }
    if(moncmdl->cmd != NULL) {
        FREE(moncmdl->cmd);
    }
    if(moncmdl != NULL) {
        FREE(moncmdl);
    }
}

void monitor()
{
    char *buf, *p;
    MONCMDLINE *moncmdl;
    MONCMDTYPE cmdtype = MONREPEAT;

    do {
        fprintf(stdout, "- ");
        buf = malloc_chk(MONINSIZE + 1, "monitor.buf");
        fgets(buf, MONINSIZE, stdin);
        if((p = strchr(buf, '\n')) != NULL) {
            *p = '\0';
        }
        if((moncmdl = monlinetok(buf)) != NULL) {
            cmdtype = monitorcmd(moncmdl->cmd, moncmdl->args);
            free_moncmdline(moncmdl);
        }
        FREE(buf);
        if(cmdtype == MONQUIT) {
            shutdown();
            freebps();
            free_code_cmdtype();
            freecerr();
            exit(0);
        }
    } while(cmdtype == MONREPEAT);
}
