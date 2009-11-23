#include "casl2.h"
#include "assemble.h"

/*マクロ命令「IN IBUF,LEN」をメモリに書込  
	PUSH 0,GR1
	PUSH 0,GR2
	LAD GR1,IBUF
	LAD GR2,LEN
        SVC 1
	POP GR2
	POP GR1
*/
bool writeIN(const char *ibuf, const char *len, PASS pass)
{
    bool status = false;

    /* PUSH 0,GR1 */
    writememory(0x7001, ptr++, pass);
    writememory(0x0, ptr++, pass);
    /* PUSH 0,GR2 */
    writememory(0x7002, ptr++, pass);
    writememory(0x0, ptr++, pass);
    /* LAD GR1,IBUF */
    writememory(0x1210, ptr++, pass);
    writememory(getadr(ibuf, pass), ptr++, pass);
    /* LAD GR2,LEN */
    writememory(0x1220, ptr++, pass);
    writememory(getadr(len, pass), ptr++, pass);
    /* SVC 1 */
    writememory(0xF000, ptr++, pass);
    writememory(0x0001, ptr++, pass);
    /* POP GR2 */
    writememory(0x7120, ptr++, pass);
    /* POP GR1 */
    writememory(0x7110, ptr++, pass);
    if(cerrno == 0) {
        status = true;
    }
    return status;
}

/* マクロ命令「OUT OBUF,LEN」をメモリに書込
	PUSH 0,GR1
        PUSH 0,GR2
        LAD GR1,OBUF
        LD GR2,LEN
        SVC 2
	LAD GR1,=#A
        LAD GR2,1
        SVC 2
        POP GR2
        POP GR1
*/
bool writeOUT(const char *obuf, const char *len, PASS pass)
{
    bool status = false;

    /* PUSH 0,GR1 */
    writememory(0x7001, ptr++, pass);
    writememory(0x0, ptr++, pass);
    /* PUSH 0,GR2 */
    writememory(0x7002, ptr++, pass);
    writememory(0x0, ptr++, pass);
    /* LAD GR1,OBUF */
    writememory(0x1210, ptr++, pass);
    writememory(getadr(obuf, pass), ptr++, pass);
    /* LD GR2,OLEN */
    writememory(0x1020, ptr++, pass);
    writememory(getadr(len, pass), ptr++, pass);
    /* SVC 2 */
    writememory(0xF000, ptr++, pass);
    writememory(0x0002, ptr++, pass);
    /* LAD GR1,=#A */ 
    writememory(0x1210, ptr++, pass);
    if(pass == FIRST) {
        ptr++;
    } else {
        writememory(lptr, ptr++, pass);    /* リテラルのアドレスを書込 */
    }
    writememory(0xA, lptr++, pass);
    /* LAD GR2,1 */
    writememory(0x1220, ptr++, pass);
    writememory(0x0001, ptr++, pass);
    /* SVC 2 */
    writememory(0xF000, ptr++, pass);
    writememory(0x0002, ptr++, pass);
    /* POP GR2 */
    writememory(0x7120, ptr++, pass);
    /* POP GR1 */
    writememory(0x7110, ptr++, pass);
    if(cerrno == 0) {
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
        writememory(0x7000 + i, ptr++, pass);   /* PUSH GRn */
        writememory(0x0, ptr++, pass);
    }
    if(cerrno == 0) {
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
        writememory((0x7100 + (i << 4)), ptr++, pass);  /* POP GRn */
    }
    if(cerrno == 0) {
        status = true;
    }
    return status;
}
