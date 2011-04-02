#ifndef YACASL2_EXEC_INCLUDED
#define YACASL2_EXEC_INCLUDED

#include <stdbool.h>
#include "struct.h"
#include "word.h"

enum {
    INSIZE = 256    /**<IN命令の、入力領域 */
};

/**
 * 実行モード型
 */
typedef struct {
    bool trace;           /**<レジストリの内容を表示する場合はtrue */
    bool logical;         /**<レジストリの内容を論理値（0から65535）で表示する場合はtrue */
    bool dump;            /**<メモリの内容を表示する場合はtrue */
} EXECMODE;

/**
 * 実行モード: trace, logical, dump
 */
extern EXECMODE execmode;

/**
 * 実行エラーをエラーリストに追加
 */
void addcerrlist_exec();

/**
 * アセンブル結果読み込みエラーをエラーリストに追加
 */
void addcerrlist_load();

/**
 * 指定されたファイルからアセンブル結果を読み込む
 */
bool loadassemble(const char *file);

/**
 * COMET II仮想マシンのリセット
 */
void reset();

/**
 * NOP命令。語長1（OPのみ）
 */
void nop();

/**
 * LD命令 - オペランドr,adr,x。語長2
 */
void ld_r_adr_x();

/**
 * LD命令 - オペランドr1,r2。語長1
 */
void ld_r1_r2();

/**
 * ST命令。語長2
 */
void st();

/**
 * LAD命令。語長2
 */
void lad();

/**
 * ADDA命令 - オペランドr,adr,x。語長2
 */
void adda_r_adr_x();

/**
 * ADDA命令 - オペランドr1,r2。語長1
 */
void adda_r1_r2();

/**
 * SUBA命令 - オペランドr,adr,x。語長2
 */
void suba_r_adr_x();

/**
 * SUBA命令 - オペランドr1,r2。語長1
 */
void suba_r1_r2();

/**
 * ADDL命令 - オペランドr,adr,x。語長2
 */
void addl_r_adr_x();

/**
 * ADDL命令 - オペランドr1,r2。語長1
 */
void addl_r1_r2();

/**
 * SUBL命令 - オペランドr,adr,x。語長2
 */
void subl_r_adr_x();

/**
 * SUBL命令 - オペランドr1,r2。語長1
 */
void subl_r1_r2();

/**
 * AND命令 - オペランドr,adr,x。語長2
 */
void and_r_adr_x();

/**
 * AND命令 - オペランドr1,r2。語長1
 */
void and_r1_r2();

/**
 * OR命令 - オペランドr,adr,x。語長2
 */
void or_r_adr_x();

/**
 * OR命令 - オペランドr1,r2。語長1
 */
void or_r1_r2();

/**
 * XOR命令 - オペランドr,adr,x。語長2
 */
void xor_r_adr_x();

/**
 * XOR命令 - オペランドr1,r2。語長1
 */
void xor_r1_r2();

/**
 * CPA命令 - オペランドr,adr,x。語長2
 */
void cpa_r_adr_x();

/**
 * CPA命令 - オペランドr1,r2。語長1
 */
void cpa_r1_r2();

/**
 * CPL命令 - オペランドr,adr,x。語長2
 */
void cpl_r_adr_x();

/**
 * CPL命令 - オペランドr1,r2。語長1
 */
void cpl_r1_r2();

/**
 * SLA命令 - オペランドr,adr,x。語長2
 * 算術演算なので、第15ビットは送り出されない
 */
void sla();

/**
 * SRA命令 - オペランドr,adr,x。語長2
 * 算術演算なので、第15ビットは送り出されない
 * 空いたビット位置には符号と同じものが入る
 */
void sra();

/**
 * SLL命令 - オペランドr,adr,x。語長2
 */
void sll();

/**
 * SRL命令 - オペランドr,adr,x。語長2
 */
void srl();

/**
 * JPL命令。語長2
 */
void jpl();

/**
 * JMI命令。語長2
 */
void jmi();

/**
 * JNZ命令。語長2
 */
void jnz();

/**
 * JZE命令。語長2
 */
void jze();

/**
 * JOV命令。語長2
 */
void jov();

/**
 * JUMP命令。語長2
 */
void jump();

/**
 * PUSH命令。語長2
 */
void push();

/**
 * POP命令。語長1
 */
void pop();

/**
 * CALL命令。語長2
 */
void call();

/**
 * RET命令。語長1（OPのみ）
 */
void ret();

/**
 * SVC命令。語長2
 */
void svc();

/**
 * COMET II仮想マシンの実行
 */
bool exec();

/**
 * COMET IIのメモリを表示
 */
void dumpmemory();

/**
 * COMET IIのレジスタを表示
 */
void dspregister();

#endif            /* YACASL2_EXEC_INCLUDEDの終端 */
