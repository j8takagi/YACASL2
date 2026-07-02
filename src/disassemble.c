#include "disassemble.h"

/**
 * @brief 機械コードの出力列
 */
int codecol = 32;

/**
 * @brief ファイルストリームから1ワードを取得する
 *
 * @return 取得した1ワード
 *
 * @param stream ファイルストリーム
 */
WORD fgetword(FILE *stream);

/**
 * @brief WORDデータから、値が0のWORDがいくつ連続するか返す
 *
 * @return 値が0の連続するWORD数
 *
 * @param data WORDデータ
 * @param wordlen データのWORD数
 */
WORD zero_data_cnt(const WORD *data, WORD wordlen);

/**
 * @brief 機械コードをコメントとして標準出力へ出力する
 *
 * @param ascol アセンブラの列位置
 * @param pradr 次に実行すべき命令語の先頭アドレス
 * @param wordc ワード値の数
 * @param wordv ワード値の配列
 */
void disassemble_puts_code(int ascol, WORD pradr, int wordc, WORD wordv[]);

/**
 * @brief 種類がR_ADRまたはR_ADR_Xのコマンドを逆アセンブルし、標準出力へ出力する
 *
 * @param cmdtype コマンドの種類
 * @param *cmdname コマンドの名前
 * @param word ワード値
 * @param adr アドレス値
 * @param pradr 次に実行すべき命令語の先頭アドレス
 */
void disassemble_cmd_adr_x(CMDTYPE cmdtype, const char *cmdname, WORD word, WORD adr, WORD pradr);

/**
 * @brief 種類がR1_R2またはR_のコマンドを逆アセンブルし、標準出力へ出力する
 *
 * @param cmdtype コマンドの種類
 * @param *cmdname コマンドの名前
 * @param word ワード値
 * @param pradr 次に実行すべき命令語の先頭アドレス
 */
void disassemble_cmd_r(CMDTYPE cmdtype, const char *cmdname, WORD word, WORD pradr);

/**
 * @brief DCコマンドを逆アセンブルし、標準出力へ出力する
 *
 * @param word ワード値
 * @param pradr 次に実行すべき命令語の先頭アドレス
 */
void disassemble_dc(WORD word, WORD pradr);

void disassemble_puts_code(int ascol, WORD pradr, int wordc, WORD wordv[])
{
    for(int i = 0; i < codecol-ascol; i++){
        fprintf(stdout, " ");
    }
    if(wordc == 1) {
        fprintf(stdout, "; #%04X: #%04X", pradr, wordv[0]);
    } else if(wordc == 2) {
        fprintf(stdout, "; #%04X: #%04X #%04X", pradr, wordv[0], wordv[1]);
    }
}

void disassemble_cmd_adr_x(CMDTYPE cmdtype, const char *cmdname, WORD word, WORD adr, WORD pradr)
{
    WORD x = 0;
    char *g = NULL;
    int cnt = 0;

    cnt += fprintf(stdout, "        %-7s ", cmdname);
    if(cmdtype == R_ADR_X) {
        cnt += fprintf(stdout, "%s,", g = grstr((word & 0x00F0) >> 4));
        FREE(g);
    }
    cnt += fprintf(stdout, "#%04X", adr);
    if((x = (word & 0x000F)) != 0) {
        cnt += fprintf(stdout, ",%s", g = grstr(x));
        FREE(g);
    }
    disassemble_puts_code(cnt, pradr, 2, (WORD []){word, adr});
}

void disassemble_cmd_r(CMDTYPE cmdtype, const char *cmdname, WORD word, WORD pradr)
{
    char *g = NULL;
    char *g1 = NULL;
    char *g2 = NULL;
    int cnt = 0;

    cnt += fprintf(stdout, "        %-7s ", cmdname);
    if(cmdtype == R1_R2) {
        g1 = grstr((word & 0x00F0) >> 4);
        g2 = grstr(word & 0x000F);
        cnt += fprintf(stdout, "%s,%s", g1, g2);
        FREE(g1);
        FREE(g2);
    } else if(cmdtype == R_) {
        g = grstr((word & 0x00F0) >> 4);
        cnt += fprintf(stdout, "%s", g);
        FREE(g);
    }
    disassemble_puts_code(cnt, pradr, 1, (WORD []){word});
}

void disassemble_dc(WORD word, WORD pradr)
{
    int cnt = 0;

    cnt = fprintf(stdout, "        DC      %-5d ", word);
    disassemble_puts_code(cnt, pradr, 1, (WORD []){word});
    fprintf(stdout, " ::" );
    print_dumpword(word, true);
}

void disassemble_ds(WORD wcnt, WORD pradr)
{
    int cnt = 0;

    cnt = fprintf(stdout, "        DS      %-5d ", wcnt);
    disassemble_puts_code(cnt, pradr, 1, (WORD []){0});
    for(int i = 1; i < wcnt; i++) {
        fprintf(stdout, "\n");
        disassemble_puts_code(0, pradr+i, 1, (WORD []){0});
    }
}

WORD fgetword(FILE *stream)
{
    WORD aword;
    fread(&aword, sizeof(WORD), 1, stream);
    return aword;
}

WORD zero_data_cnt(const WORD *data, WORD wordlen)
{
    WORD cnt = 0;
    for(cnt = 0; cnt < wordlen && data[cnt] == 0; cnt++) {
        ;
    }
    return cnt;
}

void disassemble_file(const char *file)
{
    WORD *buf = NULL;
    int len = 0;
    FILE *fp = NULL;

    assert(file != NULL);
    if((fp = fopen(file, "rb")) == NULL) {
        perror(file);
        return;
    }

    buf = calloc_chk(MAX_MEMSIZE, sizeof(WORD), "disassemble_file");
    for(int i = 0; !feof(fp) && i < MAX_MEMSIZE; i++) {
        buf[i] = fgetword(fp);
        len = i - 1;
    }
    fclose(fp);
    fprintf(stdout, "MAIN    START\n");
    disassemble_memory(buf, 0, len);
    fprintf(stdout, "        END\n");
}

void disassemble_memory(WORD *memory, WORD start, WORD end)
{
    WORD cmd = 0;
    CMDTYPE cmdtype = 0;
    const char *cmdname = NULL;
    bool inprogram = true;
    WORD zcnt = 0;

    WORD ptr = start;
    while(ptr <= end) {
        cmd = memory[ptr] & 0xFF00;
        cmdname = getcmdname(cmd);
        cmdtype = getcmdtype(cmd);
        if((cmd > 0 && cmdname != NULL && code_gr_valid(memory[ptr]) == true) || (inprogram == true && memory[ptr] == 0)) {
            if(cmdtype == R_ADR_X || cmdtype == ADR_X) {
                disassemble_cmd_adr_x(cmdtype, cmdname, memory[ptr], memory[ptr + 1], ptr);
            } else {
                disassemble_cmd_r(cmdtype, cmdname, memory[ptr], ptr);
            }
            ptr += code2cmdwordlen(cmd);
            inprogram = (cmd != 0x8100) ? true : false;
        } else {
            if(memory[ptr] == 0 && ((zcnt = zero_data_cnt(memory + ptr, end - ptr + 1)) > 1 || ptr == end)) { /* 0が2つ以上の場合とメモリー末尾の場合は、DSとみなす */
                disassemble_ds(zcnt, ptr);
                ptr += zcnt;
            } else {
                disassemble_dc(memory[ptr], ptr);
                ptr++;
            }
        }
        fprintf(stdout, "\n");
    }
}
