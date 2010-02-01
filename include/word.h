#ifndef YACASL2_WORD_H_INCLUDED
#define YACASL2_WORD_H_INCLUDED

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include "cerr.h"

/* WORD - 16ビットデータ型 */
typedef unsigned short WORD;

/* 10進数の文字列をWORD値に変換 */
WORD n2word(const char *str);

/* 16進数の文字列をWORD値に変換 */
WORD h2word(const char *str);

/* 10進数または16進数の文字列をWORD値に変換 */
WORD nh2word(const char *str);

/* WORD値を10進数の文字列に変換 */
char *word2n(WORD word);

/* WORD値を2進数の文字列に変換 */
char *word2bit(const WORD word);

#endif
