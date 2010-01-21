#include "casl2.h"

/* エラーコードリスト */
typedef struct {
    int num;
    char *msg;
} CERRARRAY;
CERRARRAY cerr[] = {
    { 101, "label already defined" },
    { 102, "label table is full" },
    { 103, "label not found" },
    { 104, "label length is too long" },
    { 105, "no command in the line" },
    { 106, "operand count mismatch" },
    { 107, "no label in START" },
    { 108, "not command of operand \"r\"" },
    { 109, "not command of operand \"r1,r2\"" },
    { 110, "not command of operand \"r,adr[,x]\"" },
    { 111, "not command of operand \"adr[,x]\"" },
    { 112, "not command of no operand" },
    { 113, "command not defined" },
    { 114, "not integer" },
    { 115, "not hex" },
    { 116, "out of hex range" },
    { 117, "operand is too many" },
    { 118, "operand length is too long" },
    { 119, "out of COMET II memory" },
    { 120, "GR0 in operand x" },
    { 121, "cannot get operand token" },
    { 122, "cannot create hash table" },
    { 201, "execute - out of COMET II memory" },
    { 202, "SVC input - out of Input memory" },
    { 203, "SVC output - out of COMET II memory" },
    { 204, "Program Register (PR) - out of COMET II memory" },
    { 205, "Stack Pointer (SP) - cannot allocate stack buffer" },
    { 206, "Address - out of COMET II memory" },
    { 207, "Stack Pointer (SP) - out of COMET II memory" },
};

/* WORD値を文字列に変換 */
char *wtoa(WORD word)
{
    char *p = malloc(6), *q = malloc(6);
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

/* エラー番号とエラーメッセージを設定する */
void setcerr(int num, const char *val)
{
    cerrno = num;
    cerrmsg = malloc(256);
    if(val != NULL) {
        strcpy(cerrmsg, val);
        strcat(cerrmsg, ": ");
        strcat(cerrmsg, getcerrmsg(cerrno));
    } else {
        strcpy(cerrmsg, getcerrmsg(cerrno));
    }
}

/* エラー番号からメッセージを返す */
char *getcerrmsg(int num)
{
    assert(num > 0);
    int i;
    for(i = 0; i < ARRAYSIZE(cerr); i++) {
        if((&cerr[i])->num == num) {
            return (&cerr[i])->msg;
        }
    }
    return "unkown error";
}

/* エラーを解放する */
void freecerr()
{
    assert(cerrno > 0);
    cerrno = 0;
    if(strlen(cerrmsg) > 0) {
        free(cerrmsg);
    }
}
