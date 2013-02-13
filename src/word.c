#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "word.h"
#include "cerr.h"

/**
 * wordのエラー定義
 */
static CERR cerr_word[] = {
    { 114, "not integer" },
    { 115, "not hex" },
    { 116, "out of hex range" },
};

/**
 * wordのエラーをエラーリストに追加
 */
void addcerrlist_word()
{
    addcerrlist(ARRAYSIZE(cerr_word), cerr_word);
}

/**
 * 10進数の文字列をWORD値に変換
 */
WORD n2word(const char *str)
{
    assert(isdigit(*str) || *str == '-');

    char *check;
    int n;
    /* WORD値に変換 */
    n = strtol(str, &check, 10);
    if(*check != '\0') {
        setcerr(114, str);    /* not integer */
        return 0x0;
    }
    /* nが-32768から32767の範囲にないときは、その下位16ビットを格納 */
    if(n < -32768 || n > 32767) {
        n = n & 0xFFFF;
    }
    return (WORD)n;
}

/**
 * 16進数の文字列をWORD値に変換
 */
WORD h2word(const char *str)
{
    assert(*str == '#');

    WORD w = 0x0;
    char *check;
    str++;
    if(*str == '-' || strlen(str) > 4) {
        setcerr(116, str-1);    /* out of hex range */
        return 0;
    }
    /* WORD値に変換 */
    w = (WORD)strtol(str, &check, 16);
    if(*check != '\0') {
        setcerr(115, str-1);    /* not hex */
        return 0x0;
    }
    return w;
}

/**
 * 10進数または16進数の文字列をWORD値に変換
 */
WORD nh2word(const char *str)
{
    WORD w;

    assert(sizeof(WORD) * 8 == 16); /* WORD型のサイズが16ビットであることを確認 */
    if(!isdigit(*str) && *str != '-' && *str != '#') {
        setcerr(114, str);    /* not integer */
        return 0x0;
    }
    if(*str == '#') {
        w = h2word(str);
    } else {
        w = n2word(str);
    }
    return w;
}

/**
 * WORD値を10進数の文字列に変換
 */
char *word2n(WORD word)
{
    enum {
        MAXLEN = 5,        /* WORD値を10進数で表したときの最大桁数 */
    };
    char *p = malloc_chk(MAXLEN + 1, "word2n.p"), *digit = malloc_chk(MAXLEN + 1, "word2n.digit");
    int i = 0, j;

    do{
        *(p + i++) = word % 10 + '0';
    } while((word /= 10) > 0);
    for(j = 0; j < i; j++) {
        *(digit + j) = *(p + (i - 1) - j);
    }
    *(digit + j + 1) = '\0';
    FREE(p);
    return digit;
}

/**
 * WORD値を2進数の文字列に変換
 */
char *word2bit(const WORD word)
{
    enum {
        MAXLEN = 16,        /* WORD値を2進数で表したときの最大桁数 */
    };
    WORD mask = 0x8000;
    char *bit, *p;

    bit = p = malloc_chk(MAXLEN + 1, "word2bit.bit");
    do {
        *p++ = (word & mask) ? '1' : '0';
    } while((mask >>= 1) > 0);
    *p = '\0';
    return bit;
}

/**
 * WORD値を解析して表示
 */
void print_dumpword(WORD word, bool logicalmode)
{
    char *b;

    if(logicalmode == true) {
        fprintf(stdout, "%6d", word);
    } else {
        fprintf(stdout, "%6d", (signed short)word);
    }
    fprintf(stdout, " = #%04X = %s", word, (b = word2bit(word)));
    /* 「文字の組」の符号表に記載された文字と、改行（CR）／タブを表示 */
    if(word >= 0x20 && word <= 0x7E) {
        fprintf(stdout, " = \'%c\'", word);
    } else if(word == 0xA) {
        fprintf(stdout, " = \'\\n\'");
    } else if(word == '\t') {
        fprintf(stdout, " = \'\\t\'");
    }
    fprintf(stdout, "\n");
    FREE(b);
}
