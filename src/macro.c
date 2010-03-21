#include "casl2.h"
#include "assemble.h"

/* マクロ命令「IN IBUF,LEN」をメモリに書込 */
/*         PUSH 0,GR1   */
/*         PUSH 0,GR2   */
/*         LAD GR1,IBUF */
/*         LAD GR2,LEN  */
/*         SVC 1        */
/*         POP GR2      */
/*         POP GR1      */
bool writeIN(const char *ibuf, const char *len, PASS pass)
{
    bool status = false;

    /* PUSH 0,GR1 */
    writememory(0x7001, (asprop->ptr)++, pass);
    writememory(0x0, (asprop->ptr)++, pass);
    /* PUSH 0,GR2 */
    writememory(0x7002, (asprop->ptr)++, pass);
    writememory(0x0, (asprop->ptr)++, pass);
    /* LAD GR1,IBUF */
    writememory(0x1210, (asprop->ptr)++, pass);
    writememory(getadr(asprop->prog, ibuf, pass), (asprop->ptr)++, pass);
    /* LAD GR2,LEN */
    writememory(0x1220, (asprop->ptr)++, pass);
    writememory(getadr(asprop->prog, len, pass), (asprop->ptr)++, pass);
    /* SVC 1 */
    writememory(0xF000, (asprop->ptr)++, pass);
    writememory(0x0001, (asprop->ptr)++, pass);
    /* POP GR2 */
    writememory(0x7120, (asprop->ptr)++, pass);
    /* POP GR1 */
    writememory(0x7110, (asprop->ptr)++, pass);
    if(cerr->num == 0) {
        status = true;
    }
    return status;
}

/* マクロ命令「OUT OBUF,LEN」をメモリに書込 */
/*         PUSH 0,GR1   */
/*         PUSH 0,GR2   */
/*         LAD GR1,OBUF */
/*         LAD GR2,LEN  */
/*         SVC 2        */
/*         LAD GR1,=#A  */
/*         LAD GR2,1    */
/*         SVC 2        */
/*         POP GR2      */
/*         POP GR1      */
bool writeOUT(const char *obuf, const char *len, PASS pass)
{
    bool status = false;

    /* PUSH 0,GR1 */
    writememory(0x7001, (asprop->ptr)++, pass);
    writememory(0x0, (asprop->ptr)++, pass);
    /* PUSH 0,GR2 */
    writememory(0x7002, (asprop->ptr)++, pass);
    writememory(0x0, (asprop->ptr)++, pass);
    /* LAD GR1,OBUF */
    writememory(0x1210, (asprop->ptr)++, pass);
    writememory(getadr(asprop->prog, obuf, pass), (asprop->ptr)++, pass);
    /* LAD GR2,OLEN */
    writememory(0x1220, (asprop->ptr)++, pass);
    writememory(getadr(asprop->prog, len, pass), (asprop->ptr)++, pass);
    /* SVC 2 */
    writememory(0xF000, (asprop->ptr)++, pass);
    writememory(0x0002, (asprop->ptr)++, pass);
    /* LAD GR1,=#A */
    writememory(0x1210, (asprop->ptr)++, pass);
    if(pass == FIRST) {
        (asprop->ptr)++;
    } else {
        writememory(asprop->lptr, (asprop->ptr)++, pass);    /* リテラルのアドレスを書込 */
    }
    writememory(0xA, (asprop->lptr)++, pass);
    /* LAD GR2,=1 */
    writememory(0x1220, (asprop->ptr)++, pass);
    if(pass == FIRST) {
        (asprop->ptr)++;
    } else {
        writememory(asprop->lptr, (asprop->ptr)++, pass);    /* リテラルのアドレスを書込 */
    }
    writememory(0x1, (asprop->lptr)++, pass);
    /* SVC 2 */
    writememory(0xF000, (asprop->ptr)++, pass);
    writememory(0x0002, (asprop->ptr)++, pass);
    /* POP GR2 */
    writememory(0x7120, (asprop->ptr)++, pass);
    /* POP GR1 */
    writememory(0x7110, (asprop->ptr)++, pass);
    if(cerr->num == 0) {
        status = true;
    }
    return status;
}

/* マクロ命令「RPUSH」をメモリに書き込む
        PUSH 0,GR1
        PUSH 0,GR2
        PUSH 0,GR3
        PUSH 0,GR4
        PUSH 0,GR5
        PUSH 0,GR6
        PUSH 0,GR7
 */
bool writeRPUSH(PASS pass) {
    int i;
    bool status = false;

    for(i = 1; i <= 7; i++) {
        writememory(0x7000 + i, (asprop->ptr)++, pass);   /* PUSH GRn */
        writememory(0x0, (asprop->ptr)++, pass);
    }
    if(cerr->num == 0) {
        status = true;
    }
    return status;
}

/* マクロ命令「RPOP」をメモリに書き込む
        POP GR7
        POP GR6
        POP GR5
        POP GR4
        POP GR3
        POP GR3
        POP GR2
        POP GR1
 */
bool writeRPOP(PASS pass) {
    int i;
    bool status = false;
    for(i = 7; i >= 1; i--) {
        writememory((0x7100 + (i << 4)), (asprop->ptr)++, pass);  /* POP GRn */
    }
    if(cerr->num == 0) {
        status = true;
    }
    return status;
}
