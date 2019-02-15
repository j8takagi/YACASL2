#ifndef MONITOR_INCLUDE
#define MONITOR_INCLUDE

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "token.h"
#include "assemble.h"
#include "hash.h"
#include "cmem.h"
#include "cerr.h"
#include "disassemble.h"
#include "word.h"

/**
 * @brief モニター
 */
enum {
    MONARGSIZE = 3,          /**<モニター引数の最大数 */
};

/**
 * @brief モニター引数を表すデータ型
 */
typedef struct {
    int argc;                   /**<オペランド数 */
    char *argv[MONARGSIZE];      /**<オペランド配列 */
} MONARGS;

/**
 * @brief モニター命令行を表すデータ型
 */
typedef struct {
    char *cmd;                  /**<コマンド */
    MONARGS *args;               /**<引数 */
} MONCMDLINE;

/**
 * @brief ブレークポイント表を表すデータ型
 */
typedef struct _BPSLIST {
    struct _BPSLIST *next;        /**<リスト次項目へのポインタ */
    WORD adr;                   /**<アドレス */
} BPSLIST;

/**
 * ブレークポイント表のサイズ
 */
enum {
    BPSTABSIZE = 251,         /**<ブレークポイント表のサイズ */
};

enum {
    MONINSIZE = 40    /**<モニターの、入力領域 */
};

typedef enum {
    MONREPEAT = 0,
    MONNEXT = 1,
    MONQUIT = 2,
} MONCMDTYPE;

/**
 * @brief アドレスのハッシュ値を返す
 *
 * @return ハッシュ値
 *
 * @param adr アドレス
 */
unsigned adrhash(WORD adr);

/**
 * @brief 文字列から、モニターの引数を取得する
 *
 * @return モニターの引数
 *
 * @param *str 文字列
 */
MONARGS *monargstok(const char *str);

/**
 * @brief 行から、モニターの命令と引数を取得する
 *
 * @return モニターの命令と引数
 *
 * @param *line 行
 */
MONCMDLINE *monlinetok(const char *line);

/**
 * @brief モニターの命令を実行する
 *
 * @return モニター命令の種類
 *
 * @param *cmd モニター命令
 * @param *args モニター命令の引数
 */
MONCMDTYPE monitorcmd(char *cmd, MONARGS *args);

/**
 * @brief ブレークポイント表にアドレスがある場合はtrue、ない場合はfalseを返す
 *
 * @return trueまたはfalse
 *
 * @param *adr アドレス
 */
bool getbps(WORD adr);

/**
 * @brief ブレークポイント表にアドレスを追加する
 *
 * @return 追加した場合はtrue、追加しなかった場合はfalse
 *
 * @param *adr アドレス
 */
bool addbps(WORD adr);


/**
 * @brief ブレークポイント表からアドレスを削除する
 *
 * @return 削除した場合はtrue、削除しなかった場合はfalse
 *
 * @param *adr アドレス
 */
bool delbps(WORD adr);

/**
 * @brief ブレークポイント表を解放する
 *
 * @return なし
 */
void freebps();

/**
 * @brief COMET IIモニターを起動する
 *
 * @return なし
 */
void monitor();

#endif        /* end of MONITOR_INCLUDE */
