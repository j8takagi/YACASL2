#ifndef YACASL2_DISASSEMBLE_H_INCLUDE
#define YACASL2_DISASSEMBLE_H_INCLUDE

#include "struct.h"

/**
 * @brief CASL IIのオブジェクトファイルを逆アセンブルし、標準出力へ出力する
 *
 * @return 正常終了時は0、異常終了時は0以外
 *
 * @param *file オブジェクトファイルのファイル名
 */
bool disassemble_file(const char *file);

/**
 * @brief COMET IIのメモリーを逆アセンブルし、標準出力へ出力する
 *
 * @return なし
 *
 * @param *start 逆アセンブルの開始位置
 * @param *end 逆アセンブルの終了位置
 */
void disassemble_memory(WORD start, WORD end);

#endif
