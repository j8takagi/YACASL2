#include <stdio.h>
#include "cerr.h"

CERR cerr_0[] = {
    { 126, "source file is not specified" },
};

CERR cerr_1[] = {
    { 101, "label already defined" },
    { 102, "label table is full" },
    { 103, "label not found" },
    { 104, "label length is too long" },
    { 105, "no command in the line" },
    { 106, "operand mismatch in assemble command" },
    { 107, "no label in START" },
    { 108, "not command of operand \"r\"" },
    { 109, "not command of operand \"r1,r2\"" },
    { 110, "not command of operand \"r,adr[,x]\"" },
    { 111, "not command of operand \"adr[,x]\"" },
    { 112, "not command of no operand" },
    { 113, "operand too many in COMET II command" },
    { 117, "operand too many in DC" },
    { 118, "operand length too long" },
    { 119, "out of COMET II memory" },
    { 120, "GR0 in operand x" },
    { 121, "cannot get operand token" },
    { 122, "cannot create hash table" },
    { 123, "unclosed quote" },
    { 124, "more than one character in literal" },
    { 125, "not GR in operand x" },
};

CERR cerr_2[] = {
    { 114, "not integer" },
    { 115, "not hex" },
    { 116, "out of hex range" },
};

CERR cerr_3[] = {
    { 114, "not integer" },
    { 115, "not hex" },
    { 116, "out of hex range" },
};

CERR cerr_4[] = {
    { 201, "Load object file - full of COMET II memory" },
};

CERR cerr_5[] = {
    { 202, "SVC input - out of Input memory" },
    { 203, "SVC output - out of COMET II memory" },
    { 204, "Program Register (PR) - out of COMET II memory" },
    { 205, "Stack Pointer (SP) - cannot allocate stack buffer" },
    { 206, "Address - out of COMET II memory" },
    { 207, "Stack Pointer (SP) - out of COMET II memory" },
};

int main(){
    CERRLIST *p;
    /* エラーの追加 */
    addcerrlist(ARRAYSIZE(cerr_0), cerr_0);
    addcerrlist(ARRAYSIZE(cerr_1), cerr_1);
    addcerrlist(ARRAYSIZE(cerr_2), cerr_2);
    addcerrlist(ARRAYSIZE(cerr_3), cerr_3);
    addcerrlist(ARRAYSIZE(cerr_4), cerr_4);
    addcerrlist(ARRAYSIZE(cerr_5), cerr_5);
    for(p = cerrlist; p != NULL; p = p->next) {
        printf("%d: %s\n", p->cerr->num, p->cerr->msg);
    }
    return 0;
}
