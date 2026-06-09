#ifndef YACASL2_EXEC_H_INCLUDED
#define YACASL2_EXEC_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "struct.h"
#include "word.h"
#include "cmem.h"
#include "cerr.h"

enum {
    INSIZE = 256    /**<IN命令の、入力領域 */
};

/**
 * @brief クロック周波数を表す数値文字列を、正の整数値であるCLOCK型の数値に変換。変換できない場合は、エラーを表示して0を返す
 *
 * @return クロック周波数を表すCLOCK型の正の整数値。エラー時は0
 *
 * @param str クロック周波数を表す数値文字列
 */
CLOCK clock_str2clock(const char *str);

/**
 * @brief 実行エラーをエラーリストに追加する
 */
void addcerrlist_exec();

/**
 * @class Exec
 * @brief execから関数ポインタで呼び出される関数
 * @relatesalso exec
 **/

/**
 * @brief COMET II仮想マシンを実行する
 *
 * @relatesalso Exec
 *
 */
void exec();

/**
 * @brief NOP命令。語長1（OPのみ）
 * @relatesalso Exec
 *
 */
void nop();

/**
 * @brief LD命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void ld_r_adr_x();

/**
 * @brief LD命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 */
void ld_r1_r2();

/**
 * @brief ST命令。語長2
 * @relatesalso Exec
 *
 */
void st();

/**
 * @brief LAD命令。語長2
 * @relatesalso Exec
 *
 */
void lad();

/**
 * @brief ADDA命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void adda_r_adr_x();

/**
 * @brief ADDA命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 */
void adda_r1_r2();

/**
 * @brief SUBA命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void suba_r_adr_x();

/**
 * @brief SUBA命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 */
void suba_r1_r2();

/**
 * @brief ADDL命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void addl_r_adr_x();

/**
 * @brief ADDL命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 */
void addl_r1_r2();

/**
 * @brief SUBL命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void subl_r_adr_x();

/**
 * @brief SUBL命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 */
void subl_r1_r2();

/**
 * @brief AND命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void and_r_adr_x();

/**
 * @brief AND命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 */
void and_r1_r2();

/**
 * @brief OR命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void or_r_adr_x();

/**
 * @brief OR命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 */
void or_r1_r2();

/**
 * @brief XOR命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void xor_r_adr_x();

/**
 * @brief XOR命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 */
void xor_r1_r2();

/**
 * @brief CPA命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void cpa_r_adr_x();

/**
 * @brief CPA命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 */
void cpa_r1_r2();

/**
 * @brief CPL命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void cpl_r_adr_x();

/**
 * @brief CPL命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 */
void cpl_r1_r2();

/**
 * @brief SLA命令 - オペランドr,adr,x。語長2
 *
 * 算術演算なので、第15ビットは送り出されない
 * @relatesalso Exec
 *
 */
void sla();

/**
 * @brief SRA命令 - オペランドr,adr,x。語長2
 *
 * 算術演算なので、第15ビットは送り出されない
 * 空いたビット位置には符号と同じものが入る
 * @relatesalso Exec
 *
 */
void sra();

/**
 * @brief SLL命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void sll();

/**
 * @brief SRL命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 */
void srl();

/**
 * @brief JPL命令。語長2
 * @relatesalso Exec
 *
 */
void jpl();

/**
 * @brief JMI命令。語長2
 * @relatesalso Exec
 *
 */
void jmi();

/**
 * @brief JNZ命令。語長2
 * @relatesalso Exec
 *
 */
void jnz();

/**
 * @brief JZE命令。語長2
 * @relatesalso Exec
 *
 */
void jze();

/**
 * @brief JOV命令。語長2
 * @relatesalso Exec
 *
 */
void jov();

/**
 * @brief JUMP命令。語長2
 * @relatesalso Exec
 *
 */
void jump();

/**
 * @brief PUSH命令。語長2
 * @relatesalso Exec
 *
 */
void push();

/**
 * @brief POP命令。語長1
 * @relatesalso Exec
 *
 */
void pop();

/**
 * @brief CALL命令。語長2
 * @relatesalso Exec
 *
 */
void call();

/**
 * @brief RET命令。語長1（OPのみ）
 * @relatesalso Exec
 *
 */
void ret();

/**
 * @brief SVC命令。語長2
 * @relatesalso Exec
 *
 */
void svc();

/**
 * @brief COMET IIのメモリを表示する
 *
 * @param start 表示の開始アドレス
 * @param end 表示の終了アドレス
 */
void dumpmemory(WORD start, WORD end);

/**
 * @brief COMET IIのレジスタを表示する
 *
 */
void dspregister();

#endif
