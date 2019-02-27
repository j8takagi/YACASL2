#ifndef YACASL2_EXEC_INCLUDED
#define YACASL2_EXEC_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "struct.h"
#include "word.h"
#include "cmem.h"
#include "cerr.h"
#include "monitor.h"
#include "disassemble.h"

enum {
    INSIZE = 256    /**<IN命令の、入力領域 */
};

/**
 * @brief 実行エラーをエラーリストに追加する
 */
void addcerrlist_exec();

/**
 * @brief 指定されたファイルからアセンブル結果を読み込む
 *
 * @return 読み込み終了アドレス。読み込めなかった場合は、0
 *
 * @param file 読み込むファイル名
 * @param start 読み込み開始アドレス
 */
WORD loadassemble(const char *file, WORD start);

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
 * @return なし
 */
void exec();

/**
 * @brief NOP命令。語長1（OPのみ）
 * @relatesalso Exec
 *
 * @return なし
 */
void nop();

/**
 * @brief LD命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void ld_r_adr_x();

/**
 * @brief LD命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 * @return なし
 */
void ld_r1_r2();

/**
 * @brief ST命令。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void st();

/**
 * @brief LAD命令。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void lad();

/**
 * @brief ADDA命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void adda_r_adr_x();

/**
 * @brief ADDA命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 * @return なし
 */
void adda_r1_r2();

/**
 * @brief SUBA命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void suba_r_adr_x();

/**
 * @brief SUBA命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 * @return なし
 */
void suba_r1_r2();

/**
 * @brief ADDL命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void addl_r_adr_x();

/**
 * @brief ADDL命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 * @return なし
 */
void addl_r1_r2();

/**
 * @brief SUBL命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void subl_r_adr_x();

/**
 * @brief SUBL命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 * @return なし
 */
void subl_r1_r2();

/**
 * @brief AND命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void and_r_adr_x();

/**
 * @brief AND命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 * @return なし
 */
void and_r1_r2();

/**
 * @brief OR命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void or_r_adr_x();

/**
 * @brief OR命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 * @return なし
 */
void or_r1_r2();

/**
 * @brief XOR命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void xor_r_adr_x();

/**
 * @brief XOR命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 * @return なし
 */
void xor_r1_r2();

/**
 * @brief CPA命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void cpa_r_adr_x();

/**
 * @brief CPA命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 * @return なし
 */
void cpa_r1_r2();

/**
 * @brief CPL命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void cpl_r_adr_x();

/**
 * @brief CPL命令 - オペランドr1,r2。語長1
 * @relatesalso Exec
 *
 * @return なし
 */
void cpl_r1_r2();

/**
 * @brief SLA命令 - オペランドr,adr,x。語長2
 *
 * 算術演算なので、第15ビットは送り出されない
 * @relatesalso Exec
 *
 * @return なし
 */
void sla();

/**
 * @brief SRA命令 - オペランドr,adr,x。語長2
 *
 * 算術演算なので、第15ビットは送り出されない
 * 空いたビット位置には符号と同じものが入る
 * @relatesalso Exec
 *
 * @return なし
 */
void sra();

/**
 * @brief SLL命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void sll();

/**
 * @brief SRL命令 - オペランドr,adr,x。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void srl();

/**
 * @brief JPL命令。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void jpl();

/**
 * @brief JMI命令。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void jmi();

/**
 * @brief JNZ命令。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void jnz();

/**
 * @brief JZE命令。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void jze();

/**
 * @brief JOV命令。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void jov();

/**
 * @brief JUMP命令。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void jump();

/**
 * @brief PUSH命令。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void push();

/**
 * @brief POP命令。語長1
 * @relatesalso Exec
 *
 * @return なし
 */
void pop();

/**
 * @brief CALL命令。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void call();

/**
 * @brief RET命令。語長1（OPのみ）
 * @relatesalso Exec
 *
 * @return なし
 */
void ret();

/**
 * @brief SVC命令。語長2
 * @relatesalso Exec
 *
 * @return なし
 */
void svc();

/**
 * @brief COMET IIのメモリを表示する
 *
 * @return なし
 *
 * @param start 表示の開始アドレス
 * @param end 表示の終了アドレス
 */
void dumpmemory(WORD start, WORD end);

/**
 * @brief COMET IIのレジスタを表示する
 *
 * @return なし
 */
void dspregister();

#endif            /* YACASL2_EXEC_INCLUDEDの終端 */
