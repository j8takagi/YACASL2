#ifndef YACASL2_WORD_H_INCLUDED
#define YACASL2_WORD_H_INCLUDED

#include <stdbool.h>

/**
 * @brief 16ビットの数値を表すデータ型
 */
typedef unsigned short WORD;

/**
 * @brief wordデータ型についてのエラーをエラーリストに追加する
 *
 * @return なし
 */
void addcerrlist_word();

/**
 * @brief 10進数または16進数の文字列をWORD値に変換する
 *
 * @return WORD値
 *
 * @param *str 10進数または16進数の文字列
 */
WORD nh2word(const char *str);

/**
 * @brief WORD値を10進数値を表す文字列に変換する
 *
 * @return 10進数値を表す文字列
 *
 * @param *word WORD値
 */
char *word2n(WORD word);

/**
 * @brief WORD値を2進数の文字列に変換する
 *
 * @return 2進数値を表す文字列
 *
 * @param word WORD値
 */
char *word2bit(const WORD word);

/**
 * @brief WORD値を解析して表示する
 *
 * @return なし
 *
 * @param word WORD値
 * @param logicalmode 論理演算の場合はtrue、算術演算の場合はfalse
 */
void print_dumpword(WORD word, bool logicalmode);

#endif
