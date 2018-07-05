#include "disassemble.h"

/**
 * @brief 機械コードの出力列
 */
int codecol = 38;

/**
 * @brief 機械コードをコメントとして標準出力へ出力する
 *
 * @return なし
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
 * @return なし
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
 * @return なし
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
 * @return なし
 *
 * @param cmdtype コマンドの種類
 * @param *cmdname コマンドの名前
 * @param word ワード値
 * @param adr アドレス値
 * @param pradr 次に実行すべき命令語の先頭アドレス
 */
void disassemble_dc(WORD word, WORD pradr);

void disassemble_puts_code(int ascol, WORD pradr, int wordc, WORD wordv[])
{
    int i;
    for(i = 0; i < codecol-ascol; i++){
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
    WORD x;
    char *g;
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
    char *g, *g1, *g2;
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
    int cnt = 0, i;
    cnt = fprintf(stdout, "        DS      %-5d ", wcnt);
    disassemble_puts_code(cnt, pradr, 1, (WORD []){0});
    for(i = 0; i < wcnt - 1; i++) {
        fprintf(stdout, "\n");
        disassemble_puts_code(0, pradr+1, 1, (WORD []){0});
    }
}

bool disassemble_file(const char *file)
{
    bool stat = true;
    FILE *fp;
    WORD i = 0, w, cmd, adr, dsarea;
    CMDTYPE cmdtype = 0;
    char *cmdname;
    bool inst = true;

    assert(file != NULL);
    if((fp = fopen(file, "rb")) == NULL) {
        perror(file);
        return false;
    }

    create_code_cmdtype();                          /* 命令のコードとタイプがキーのハッシュ表を作成 */

    fprintf(stdout, "MAIN    START\n");
    for(fread(&w, sizeof(WORD), 1, fp); !feof(fp); i++, fread(&w, sizeof(WORD), 1, fp)) {
        cmd = w & 0xFF00;
        cmdname = getcmdname(cmd);
        cmdtype = getcmdtype(cmd);
        if(w == 0){
            if(inst == true) {
                disassemble_cmd_r(NONE, "nop", 0, i);
            } else {
                dsarea = 1;
                do {
                    fread(&w, sizeof(WORD), 1, fp);
                    dsarea++;
                } while(w == 0 && !feof(fp));
                if(!feof(fp)) {
                    fseek(fp, -1 * sizeof(WORD) , SEEK_CUR);
                }
                if(dsarea == 1) {
                    disassemble_dc(0, i);
                } else {
                    disassemble_ds(dsarea, i);
                    i += dsarea - 1;
                }
            }
        } else if(cmd == 0) {
            disassemble_dc(w, i);
        } else {
            if(cmdtype == R_ADR_X || cmdtype == ADR_X) {
                fread(&adr, sizeof(WORD), 1, fp);
                disassemble_cmd_adr_x(cmdtype, cmdname, w, adr, i++);
            } else {
                disassemble_cmd_r(cmdtype, cmdname, w, i);
            }
            inst = (cmd != 0x8100) ? true : false;
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "        END\n");
    free_code_cmdtype();
    fclose(fp);
    return stat;
}

void disassemble_memory(WORD start, WORD end)
{
    WORD i, w, cmd, adr;
    CMDTYPE cmdtype = 0;
    char *cmdname;
    bool inst = true;

    for(i = start; i <= end; i++) {
        w = sys->memory[i];
        cmd = w & 0xFF00;
        cmdname = getcmdname(cmd);
        cmdtype = getcmdtype(cmd);
        if(w == 0) {
            if(inst == true) {
                disassemble_cmd_r(NONE, "nop", 0, i);
            } else {
                disassemble_dc(0, i);
            }
        } else if(cmd == 0) {
            disassemble_dc(w, i);
        } else {
            if(cmdtype == R_ADR_X || cmdtype == ADR_X) {
                adr = sys->memory[i+1];
                disassemble_cmd_adr_x(cmdtype, cmdname, w, adr, i++);
            } else {
                disassemble_cmd_r(cmdtype, cmdname, w, i);
            }
            inst = (cmd != 0x8100) ? true : false;
        }
        fprintf(stdout, "\n");
    }
}
