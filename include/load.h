#ifndef YACASL2_LOAD_H_INCLUDE
#define YACASL2_LOAD_H_INCLUDE

/**
 * @brief アセンブル結果読み込みエラーをエラーリストに追加する
 */
void addcerrlist_load();

/**
 * @brief 指定されたファイルからアセンブル結果を読み込む
 *
 * @return 読み込み終了アドレス。読み込めなかった場合は、0
 *
 * @param file 読み込むファイル名
 * @param start 読み込み開始アドレス
 */
WORD loadassemble(const char *file, WORD start);

#endif
