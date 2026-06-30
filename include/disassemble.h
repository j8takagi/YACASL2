#ifndef YACASL2_DISASSEMBLE_H_INCLUDED
#define YACASL2_DISASSEMBLE_H_INCLUDED

#include "word.h"
#include "struct.h"

/**
 * @brief CASL IIのオブジェクトファイルを逆アセンブルし、標準出力へ出力する
 *
 * @param *file オブジェクトファイルのファイル名
 */
void disassemble_file(const char *file);

/**
 * @brief メモリーを逆アセンブルし、標準出力へ出力する
 *
 * @param *memory メモリー
 * @param *start 逆アセンブルの開始位置
 * @param *end 逆アセンブルの終了位置
 */
void disassemble_memory(WORD *memory, WORD start, WORD end);

#endif
