#include "monitor.h"

/**
 * @brief ブレークポイント表
 */
static BPSLIST *bps[BPSTABSIZE];

/**
 * @brief comet2monitorのプロンプト
 */
static char *monitor_prompt = "(comet2 monitor)";

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
    BPSLIST *p = NULL;

    for(p = bps[adrhash(adr)]; p != NULL; p = p->next) {
        if(p->adr == adr) {
            return true;
        }
    }
    return false;
}

bool addbps(WORD adr)
{
    BPSLIST *p = NULL;
    unsigned h = 0;

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
    BPSLIST *p = NULL;
    BPSLIST *q = NULL;
    unsigned h = 0;
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
    int cnt = 0;
    BPSLIST *p = NULL;

    fprintf(stdout, "List of breakpoints\n");
    for(int i = 0; i < BPSTABSIZE; i++) {
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
    BPSLIST *p = NULL;
    BPSLIST *q = NULL;
    for(p = head; p != NULL; p = q) {
        q = p->next;
        FREE(p);
    }
}

void freebps()
{
    for(int i = 0; i < BPSTABSIZE; i++) {
        freebpslist(bps[i]);
        bps[i] = NULL;
    }
}

MONARGS *monargstok(const char *str)
{
    MONARGS *args = malloc_chk(sizeof(MONARGS), "args");
    char *tok = NULL;
    char *p = NULL;
    char sepc = ' ';

    args->argc = 0;
    if(!str || !str[0]) {
        return args;
    }
    tok = p = strdup_chk(str, "argstok.p");
    do {
        int i = strcspn(p, " ");
        sepc = p[i];
        args->argv[(args->argc)++] = strndup_chk(p, i, "args->argv[]");
        p += i + 1;
    } while(sepc == ' ');
    FREE(tok);
    return args;
}

MONCMDLINE *monlinetok(const char *line)
{
    char *tokens = NULL;
    char *p = NULL;
    int i = 0;
    MONCMDLINE *moncmdl = NULL;

    if(!line[0] || line[0] == '\n') {
        return NULL;
    }
    p = tokens = strdup_chk(line, "tokens");
    /* コメントを削除 */
    strip_casl2_comment(p);
    /* 文字列末尾の改行と空白を削除 */
    strip_end(p);

    moncmdl = malloc_chk(sizeof(MONCMDLINE), "moncmdl");
    /* コマンドの取得 */
    i = strcspn(p, " \t\n");
    moncmdl->cmd = strndup_chk(p, i, "moncmdl->cmd");
    /* コマンドと引数の間の空白をスキップ */
    p += i;
    while(*p == ' ' || *p == '\t') {
        p++;
    }
    /* 引数として、改行までの文字列を取得 */
    if(strcspn(p, "\n") > 0) {
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

void warn_ignore_arg(int argc, char *argv[])
{
    int i;
    for(i = 0; i < argc; i++) {
        if(i > 0) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "%s", argv[i]);
    }
    fprintf(stderr, ": ignored.\n");
}

void mon_break(int argc, char *argv[])
{
    WORD w;
    int i = 0;
    if(stracmp(argv[0], 2, (char* []){"l", "list"})) {
        i++;
        listbps();
    } else if(stracmp(argv[0], 2, (char* []){"r", "reset"})) {
        i++;
        freebps();
        fprintf(stdout, "All breakpoints are deleted.\n");
    } else {
        if(argc > 1) {
            if((w = nh2word(argv[1])) == 0x0) {
                fprintf(stderr, "%s: address error\n", argv[1]);
            }
        }
        if(stracmp(argv[0], 2, (char* []){"a", "add"})) {
            i += 2;
            if(addbps(w) == true) {
                fprintf(stdout, "#%04X: breakpoint added\n", w);
            } else {
                fprintf(stdout, "No breakpoint added\n");
            }
        } else if(stracmp(argv[0], 2, (char* []){"d", "del"})) {
            i += 2;
            if(delbps(w) == true) {
                fprintf(stdout, "#%04X: breakpoint deleted\n", w);
            } else {
                fprintf(stdout, "No breakpoint deleted\n");
            }
        } else if(stracmp(argv[0], 3, (char* []){"?", "h", "help"})) {
            i++;
            fprintf(stdout, "breakpoint manipulate:\n");
            fprintf(stdout, "    b[reak] a[dd] <address>\n");
            fprintf(stdout, "    b[reak] d[el] <address>\n");
            fprintf(stdout, "    b[reak] l[ist]\n");
            fprintf(stdout, "    b[reak] r[eset]\n");
        } else {
            fprintf(stderr, "%s: Not breakpoint manipulate command. see `b ?'.\n", argv[0]);
        }
        if(argc > i) {
            warn_ignore_arg(argc - i, argv + i);
        }
    }
}

void mon_dump(int argc, char *argv[])
{
    int i = 0;
    WORD dump_start = 0, dump_end = 0x40;
    if(argc > 0 && stracmp(argv[0], 2, (char* []){"a", "auto"})) {
        execmode.dump = true;
        i++;
    } else if(argc > 0 && stracmp(argv[0], 2, (char* []){"no", "noauto"})) {
        execmode.dump = false;
        i++;
    }
    if(argc > i) {
        dump_start = nh2word(argv[i++]);
        if(argc > i) {
            if(argv[i][0] =='+') {
                dump_end = dump_start + nh2word(argv[i] + 1);
            } else {
                dump_end = nh2word(argv[i]);
            }
        } else {
            dump_end += dump_start;
        }
        i++;
    }
    if(argc > i) {
        warn_ignore_arg(argc - i, argv + i);
    }
    dumpmemory(dump_start, dump_end);
    execmode.dump_start = dump_start;
    execmode.dump_end = dump_end;
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
        execptr->end = loadassemble(args->argv[0], nh2word(args->argv[1]));
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

int monquit()
{
    int stat = 0;
    shutdown();
    freebps();
    free_cmdtable(HASH_CMDTYPE);
    free_cmdtable(HASH_CODE);
    if(cerr->num > 0) {
        stat = 1;
    }
    freecerr();
    return stat;
}

void monitor()
{
    char *buf = NULL;
    MONCMDLINE *moncmdl = NULL;
    MONCMDTYPE cmdtype = MONREPEAT;

    do {
        fprintf(stdout, "%s ", monitor_prompt);
        buf = malloc_chk(MONINSIZE + 1, "monitor.buf");
        fgets(buf, MONINSIZE, stdin);
        if(!buf[0]) {
            cmdtype = MONQUIT;
        }
        strip_end(buf);        /* 文字列末尾の改行と空白を削除 */
        fprintf(stdout, "%s\n", buf);
        if((moncmdl = monlinetok(buf)) != NULL) {
            cmdtype = monitorcmd(moncmdl->cmd, moncmdl->args);
            free_moncmdline(moncmdl);
        }
        FREE(buf);
        if(cmdtype == MONQUIT) {
            exit(monquit());
        }
    } while(cmdtype == MONREPEAT);
}
