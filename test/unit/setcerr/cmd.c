#include <stdio.h>
#include "casl2.h"

static CERR cerr_utest[] = {
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
    { 123, "unclosed quote" },
    { 124, "more than one character in literal" },
    { 125, "not GR in operand x" },
    { 201, "execute - out of COMET II memory" },
    { 202, "SVC input - out of Input memory" },
    { 203, "SVC output - out of COMET II memory" },
    { 204, "Program Register (PR) - out of COMET II memory" },
    { 205, "Stack Pointer (SP) - cannot allocate stack buffer" },
    { 206, "Address - out of COMET II memory" },
    { 207, "Stack Pointer (SP) - out of COMET II memory" },
};

int main(){
    int i, j;
    int code[] = {
        101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
        111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
        121, 122, 123, 124, 201, 202, 203, 204, 205, 206, 207, 999
    };
    const char *str[] = {NULL, "foobar"};
    addcerrlist(ARRAYSIZE(cerr_utest), cerr_utest);
    /* エラーの初期化 */
    cerr = malloc_chk(sizeof(CERR), "cerr");
    for(i = 0; i < ARRAYSIZE(str); i++) {
        for(j = 0; j < ARRAYSIZE(code); j++) {
            setcerr(code[j], str[i]);
            printf("%d: %s - %d\t%s\n", code[j], str[i], cerr->num, cerr->msg);
        }
    }
    freecerr();
    return 0;
}
