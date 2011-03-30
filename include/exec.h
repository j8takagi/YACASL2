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
void nop(const WORD r, const WORD v);

/**
 * LD命令
 */
void ld(const WORD r, const WORD v);

/**
 * ST命令
 */
void st(const WORD r, const WORD v);

/**
 * LAD命令
 */
void lad(const WORD r, const WORD v);

/**
 * ADDA命令
 */
void adda(const WORD r, const WORD v);

/**
 * SUBA命令
 */
void suba(const WORD r, const WORD v);

/**
 * ADDL命令
 */
void addl(const WORD r, const WORD v);

/**
 * SUBL命令
 */
void subl(const WORD r, const WORD v);

/**
 * AND命令
 */
void and(const WORD r, const WORD v);

/**
 * OR命令
 */
void or(const WORD r, const WORD v);

/**
 * XOR命令
 */
void xor(const WORD r, const WORD v);

/**
 * CPA命令
 */
void cpa(const WORD r, const WORD v);

/**
 * CPL命令
 */
void cpl(const WORD r, const WORD v);

/**
 * SLA命令
 */
void sla(const WORD r, const WORD v);

/**
 * SRA命令
 */
void sra(const WORD r, const WORD v);

/**
 * SLL命令
 */
void sll(const WORD r, const WORD v);

/**
 * SRL命令
 */
void srl(const WORD r, const WORD v);

/**
 * JMI命令
 */
void jmi(const WORD r, const WORD v);

/**
 * JNZ命令
 */
void jnz(const WORD r, const WORD v);

/**
 * JZE命令
 */
void jze(const WORD r, const WORD v);

/**
 * JUMP命令
 */
void jump(const WORD r, const WORD v);

/**
 * JPL命令
 */
void jpl(const WORD r, const WORD v);

/**
 * JOV命令
 */
void jov(const WORD r, const WORD v);

/**
 * PUSH命令
 */
void push(const WORD r, const WORD v);

/**
 * POP命令
 */
void pop(const WORD r, const WORD v);

/**
 * CALL命令
 */
void call(const WORD r, const WORD v);

/**
 * RET命令
 */
void ret(const WORD r, const WORD v);

/**
 * SVC命令
 */
void svc(const WORD r, const WORD v);


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
