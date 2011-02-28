#ifndef YACASL2_EXEC_INCLUDED
#define YACASL2_EXEC_INCLUDED

#include <stdbool.h>
#include "struct.h"
#include "word.h"

enum {
    INSIZE = 256    /**<IN命令の、入力領域 */
};

/**
 * 実行モード型
 */
typedef struct {
    bool trace;           /**<レジストリの内容を表示する場合はtrue */
    bool logical;         /**<レジストリの内容を論理値（0から65535）で表示する場合はtrue */
    bool dump;            /**<メモリの内容を表示する場合はtrue */
} EXECMODE;

/**
 * 実行モード: trace, logical, dump
 */
extern EXECMODE execmode;

/**
 * 実行エラーをエラーリストに追加
 */
bool addcerrlist_exec();

/**
 * 指定されたファイルからアセンブル結果を読み込む
 */
bool loadassemble(char *file);

/* COMET II仮想マシンのリセット */
void reset();

/**
 * COMET II仮想マシンの実行
 */
bool exec();

/**
 * COMET IIのメモリを表示
 */
void dumpmemory();

/**
 * COMET IIのレジスタを表示
 */
void dspregister();

#endif            /* YACASL2_EXEC_INCLUDEDの終端 */
