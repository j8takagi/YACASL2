#include "word.h"

/* wordのエラー定義 */
CERR cerr_word[] = {
    { 114, "not integer" },
    { 115, "not hex" },
    { 116, "out of hex range" },
};

bool addcerrlist_word()
{
    return addcerrlist(ARRAYSIZE(cerr_word), cerr_word);
}

/* 10進数の文字列をWORD値に変換 */
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
    /* nが-32768〜32767の範囲にないときは、その下位16ビットを格納 */
    if(n < -32768 || n > 32767) {
        n = n & 0xFFFF;
    }
    return (WORD)n;
}

/* 16進数の文字列をWORD値に変換 */
WORD h2word(const char *str)
{
    assert(*str == '#');

    WORD word = 0x0;
    char *check;
    str++;
    if(*str == '-' || strlen(str) > 4) {
        setcerr(116, str-1);    /* out of hex range */
        return 0;
    }
    /* WORD値に変換 */
    word = (WORD)strtol(str, &check, 16);
    if(*check != '\0') {
        setcerr(115, str-1);    /* not hex */
        return 0x0;
    }
    return word;
}

/* 10進数または16進数の文字列をWORD値に変換 */
WORD nh2word(const char *str)
{
    addcerrlist_word();
    WORD word;
    if(!isdigit(*str) && *str != '-' && *str != '#') {
        setcerr(114, str);    /* not integer */
        return 0x0;
    }
    if(*str == '#') {
        word = h2word(str);
    } else {
        word = n2word(str);
    }
    return word;
}

/* WORD値を10進数の文字列に変換 */
char *word2n(WORD word)
{
    char *p = malloc_chk(6, "word2n.p"), *q = malloc_chk(6, "word2n.q");
    int i = 0, j;
    do{
        *(p + i++) = word % 10 + '0';
    } while((word /= 10) > 0);
    for(j = 0; j < i; j++) {
        *(q + j) = *(p + (i - 1) - j);
    }
    *(q + j + 1) = '\0';
    return q;
}

/* WORD値を2進数の文字列に変換 */
char *word2bit(const WORD word)
{
    WORD mask = 0x8000;
    char *bit = malloc_chk(16 + 1, "word2bit.bit"), *p;
    p = bit;
    do {
        *p++ = (word & mask) ? '1' : '0';
    } while((mask >>= 1) > 0);
    *p = '\0';
    return bit;
}

/* WORD値を解析して表示 */
void print_dumpword(WORD word, bool logicalmode)
{
    if(logicalmode == true) {
        fprintf(stdout, "%6d", word);
    } else {
        fprintf(stdout, "%6d", (signed short)word);
    }
    fprintf(stdout, " = #%04X = %s", word, word2bit(word));
    /* 「文字の組」の符号表に記載された文字と、改行（CR）／タブを表示 */
    if(word >= 0x20 && word <= 0x7E) {
        fprintf(stdout, " = \'%c\'", word);
    } else if(word == 0xA) {
        fprintf(stdout, " = \'\\n\'");
    } else if(word == '\t') {
        fprintf(stdout, " = \'\\t\'");
    }
    fprintf(stdout, "\n");
}
