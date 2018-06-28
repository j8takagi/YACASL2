#ifndef DEBBUGER_INCLUDE
#define DEBBUGER_INCLUDE

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hash.h"
#include "cmem.h"
#include "cerr.h"
#include "exec.h"
#include "word.h"

/**
 * @brief デバッガー
 */
enum {
    DBARGSIZE = 3,          /**<デバッガー引数の最大数 */
};

/**
 * @brief デバッガー引数を表すデータ型
 */
typedef struct {
    int argc;                   /**<オペランド数 */
    char *argv[DBARGSIZE];      /**<オペランド配列 */
} DBARGS;

/**
 * @brief デバッガー命令行を表すデータ型
 */
typedef struct {
    char *cmd;                  /**<コマンド */
    DBARGS *args;               /**<引数 */
} DBCMDLINE;

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
    DBINSIZE = 40    /**<デバッガーの、入力領域 */
};

/**
 * @brief アドレスのハッシュ値を返す
 *
 * @return ハッシュ値
 *
 * @param adr アドレス
 */
unsigned adrhash(WORD adr);

/**
 * @brief 文字列から、デバッガーの引数を取得する
 *
 * @return デバッガーの引数
 *
 * @param *str 文字列
 */
DBARGS *dbargstok(const char *str);

/**
 * @brief 行から、デバッガーの命令と引数を取得する
 *
 * @return デバッガーの命令と引数
 *
 * @param *line 行
 */
DBCMDLINE *dblinetok(const char *line);

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
 * @brief ブレークポイント表からすべてのアドレスを削除する
 *
 * @return なし
 *
 * @param なし
 */
void resetbps();

/**
 * @brief ブレークポイント表を開放する
 *
 * @return なし
 *
 * @param なし
 */
void freebps();

#endif        /* end of DEBBUGER_INCLUDE */
