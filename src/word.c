#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "word.h"
#include "cerr.h"

/**
 * @brief 10進数値を表す文字列をWORD値に変換する
 *
 * @return WORD値
 *
 * @param *str 10進数値を表す文字列
 */
WORD n2word(const char *str);

/**
 * @brief 16進数の文字列をWORD値に変換する
 *
 * @return WORD値
 *
 * @param *str 16進数値を表す文字列
 */
WORD h2word(const char *str);

WORD n2word(const char *str)
{
    assert(isdigit(str[0]) || str[0] == '-');

    char *check = NULL;
    int n;
    /* WORD値に変換 */
    n = strtol(str, &check, 10);
    if(check[0]) {
        setcerr(114, str);    /* not integer */
        return 0x0;
    }
    /* nが-32768から32767の範囲にないときは、その下位16ビットを格納 */
    if(n < -32768 || n > 32767) {
        n &= 0xFFFF;
    }
    return (WORD)n;
}

WORD h2word(const char *str)
{
    assert(str[0] == '#');

    WORD w = 0;
    char *check = NULL;
    str++;
    if(*str == '-' || strlen(str) > 4) {
        setcerr(116, str-1);    /* out of hex range */
        return 0x0;
    }
    /* WORD値に変換 */
    w = (WORD)strtol(str, &check, 16);
    if(check[0]) {
        setcerr(115, str-1);    /* not hex */
        return 0x0;
    }
    return w;
}

/**
 * @brief wordのエラー定義
 */
static CERR cerr_word[] = {
    { 114, "not integer" },
    { 115, "not hex" },
    { 116, "out of hex range" },
};

/* word.hで定義された関数群 */
void addcerrlist_word()
{
    addcerrlist(ARRAYSIZE(cerr_word), cerr_word);
}

WORD nh2word(const char *str)
{
    WORD w;

    assert(sizeof(WORD) * 8 == 16); /* WORD型のサイズが16ビットであることを確認 */
    if(str == NULL) {
        return 0x0;
    }
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

char *word2n(WORD word)
{
    enum {
        MAXLEN = 5,        /* WORD値を10進数で表したときの最大けた数 */
    };
    char *s = malloc_chk(MAXLEN + 1, "word2n.n");
    char *t = NULL;
    int d = 0;                  /* けた数 */

    do{
        s[d++] = word % 10 + '0';
    } while((word /= 10) > 0);
    s[d] = '\0';
    t = strrev(s);
    FREE(s);
    return t;
}

char *word2bit(const WORD word)
{
    enum {
        MAXLEN = 16,        /* WORD値を2進数で表したときの最大桁数 */
    };
    WORD mask = 0x8000;
    char *bit = NULL;
    int i = 0;

    bit = malloc_chk(MAXLEN + 1, "word2bit.bit");
    do {
        bit[i++] = (word & mask) ? '1' : '0';
    } while((mask >>= 1) > 0);
    bit[i] = '\0';
    return bit;
}

void print_dumpword(WORD word, bool logicalmode)
{
    char *bit = word2bit(word);

    if(logicalmode == true) {
        fprintf(stdout, "%6u", word);
    } else {
        fprintf(stdout, "%6d", (signed short)word);
    }
    fprintf(stdout, " = #%04X = %s", word, bit);
    /* 「文字の組」の符号表に記載された文字と、改行（CR）／タブを表示 */
    if(word >= 0x20 && word <= 0x7E) {
        fprintf(stdout, " = \'%c\'", word);
    } else if(word == 0xA) {
        fprintf(stdout, " = \'\\n\'");
    } else if(word == '\t') {
        fprintf(stdout, " = \'\\t\'");
    }
    FREE(bit);
}
