#include "cerr.h"

/* エラー番号とエラーメッセージ */
CERR cerr_utest[] = {
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

void addcerr_utest()
{
    addcerrlist((sizeof(cerr_utest)/sizeof(cerr_utest[0])), cerr_utest);
}
