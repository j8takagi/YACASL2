#ifndef YACASL2_WORD_H_INCLUDED
#define YACASL2_WORD_H_INCLUDED

#include <stdbool.h>

/* WORD - 16ビットデータ型 */
typedef unsigned short WORD;

/**
 * wordのエラーをエラーリストに追加
 */
void addcerrlist_word();

/**
 * 10進数または16進数の文字列をWORD値に変換
 */
WORD nh2word(const char *str);

/**
 * WORD値を10進数の文字列に変換
 */
char *word2n(WORD word);

/**
 * WORD値を2進数の文字列に変換
 */
char *word2bit(const WORD word);

/**
 * WORD値を解析して表示
 */
void print_dumpword(WORD word, bool logicalmode);

#endif
