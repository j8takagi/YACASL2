#ifndef YACASL2_TOKEN_INCLUDE
#define YACASL2_TOKEN_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include "cerr.h"
#include "cmem.h"

/**
 * @brief CASL IIの仕様
 */
enum {
    LABELSIZE = 8,         /**<ラベルの最大文字数 */
    OPDSIZE = 40,          /**<オペラントの最大数。CASL IIシミュレータの制限 */
};

/**
 * @brief YACASL2の制限
 */
enum {
    LINESIZE = 1024,       /**<行の最大文字数 */
    TOKENSIZE = 256,       /**<トークンの最大文字数 */
};

/**
 * @brief オペランドを表すデータ型
 */
typedef struct {
    int opdc;                   /**<オペランド数 */
    char *opdv[OPDSIZE];        /**<オペランド配列 */
} OPD;

/**
 * @brief 命令行を表すデータ型
 */
typedef struct {
    char *label;                /**<ラベル */
    char *cmd;                  /**<コマンド */
    OPD *opd;                   /**<オペランド */
} CMDLINE;

/**
 * @brief トークン取得のエラーを追加する
 *
 * @return なし
 */
void addcerrlist_tok();

/**
 * @brief 行から、ラベル・コマンド・オペランドを取得する
 *
 * @return ラベル・コマンド・オペランド
 *
 * @param *line 行
 */
CMDLINE *linetok(const char *line);

#endif
