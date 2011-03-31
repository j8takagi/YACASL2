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
bool loadassemble(char *file);

/**
 * COMET II仮想マシンのリセット
 */
void reset();

/**
 * NOP命令
 */
void nop(const WORD r, const WORD adr);

/**
 * LD命令
 */
void ld(const WORD r, const WORD adr);

/**
 * ST命令
 */
void st(const WORD r, const WORD adr);

/**
 * LAD命令
 */
void lad(const WORD r, const WORD adr);

/**
 * ADDA命令
 */
void adda(const WORD r, const WORD adr);

/**
 * SUBA命令
 */
void suba(const WORD r, const WORD adr);

/**
 * ADDL命令
 */
void addl(const WORD r, const WORD adr);

/**
 * SUBL命令
 */
void subl(const WORD r, const WORD adr);

/**
 * AND命令
 */
void and(const WORD r, const WORD adr);

/**
 * OR命令
 */
void or(const WORD r, const WORD adr);

/**
 * XOR命令
 */
void xor(const WORD r, const WORD adr);

/**
 * CPA命令
 */
void cpa(const WORD r, const WORD adr);

/**
 * CPL命令
 */
void cpl(const WORD r, const WORD adr);

/**
 * SLA命令
 */
void sla(const WORD r, const WORD adr);

/**
 * SRA命令
 */
void sra(const WORD r, const WORD adr);

/**
 * SLL命令
 */
void sll(const WORD r, const WORD adr);

/**
 * SRL命令
 */
void srl(const WORD r, const WORD adr);

/**
 * JMI命令
 */
void jmi(const WORD r, const WORD adr);

/**
 * JNZ命令
 */
void jnz(const WORD r, const WORD adr);

/**
 * JZE命令
 */
void jze(const WORD r, const WORD adr);

/**
 * JUMP命令
 */
void jump(const WORD r, const WORD adr);

/**
 * JPL命令
 */
void jpl(const WORD r, const WORD adr);

/**
 * JOV命令
 */
void jov(const WORD r, const WORD adr);

/**
 * PUSH命令
 */
void push(const WORD r, const WORD adr);

/**
 * POP命令
 */
void pop(const WORD r, const WORD adr);

/**
 * CALL命令
 */
void call(const WORD r, const WORD adr);

/**
 * RET命令
 */
void ret(const WORD r, const WORD adr);

/**
 * SVC命令
 */
void svc(const WORD r, const WORD adr);


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
