#include "casl2.h"

/* COMET IIのメモリ */
WORD memory[MEMSIZE];

/* COMET IIのCPUレジスタ */
WORD GR[REGSIZE], SP, PR, FR;

/* エラーメッセージ */
int cerrno = 0;
char *cerrmsg;

/* レジストリの内容を表示する場合はtrue */
bool tracemode = false;

/* レジストリの内容を論理値（0〜65535）で表示する場合はTRUE */
bool logicalmode = false;

/* メモリの内容を表示する場合はtrue */
bool dumpmode = false;

/* ソースを表示する場合はtrue */
bool srcmode = false;

/* ラベル表を表示する場合はtrue */
bool labelmode = false;

/* アセンブラ詳細結果を表示する場合はtrue */
bool asdetailmode = false;

/* アセンブルだけを行う場合はtrue */
bool onlyassemblemode = false;

/* 実行開始番地 */
WORD startptr = 0x0;

/* 実行終了番地 */
WORD endptr = 0x0;

/* ハッシュ値を取得する */
unsigned hash(const char *key, int size){
    unsigned hashval;

    for(hashval = 0; *key != '\0'; key++){
        hashval = *key + 31 * hashval;
    }
    return hashval % size;
}
