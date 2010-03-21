#ifndef YACASL2_ASSEMBLE_INCLUDED
#define YACASL2_ASSEMBLE_INCLUDED

/* CASL IIの仕様 */
enum {
    LABELSIZE = 8,         /* ラベルの最大文字数 */
    OPDSIZE = 40,          /* オペラントの最大数。CASL IIシミュレータの制限 */
};

/* YACASL2の制限 */
enum {
    LINESIZE = 1024,       /* 行の最大文字数 */
    TOKENSIZE = 256,       /* トークンの最大文字数 */
};

/* アセンブルモード */
typedef struct {
    bool src;             /* ソースを表示する場合はtrue */
    bool label;           /* ラベル表を表示する場合はtrue */
    bool onlylabel;       /* ラベル表を表示して終了する場合はtrue */
    bool asdetail;        /* アセンブラ詳細結果を表示する場合はtrue */
    bool onlyassemble;    /* アセンブルだけを行う場合はtrue */
} ASMODE;

extern ASMODE asmode;

/* アセンブル時のプロパティ */
typedef struct {
    WORD ptr;     /* 現在のポインタ */
    WORD lptr;    /* リテラル（=付きの値）を格納するポインタ */
    char *prog;   /* 他のプログラムで参照する入口名 */
} ASPROP;

extern ASPROP *asprop;

/* アセンブラ命令とマクロ命令を表す番号 */
typedef enum {
    START = 01,
    END = 02,
    DS = 03,
    DC = 04,
    IN = 011,
    OUT = 012,
    RPUSH = 013,
    RPOP = 014,
} CASLCMD;

/* アセンブラ命令とマクロ命令を表す配列 */
typedef struct {
    CASLCMD cmdid;
    int opdc_min;
    int opdc_max;
    char *cmd;
} CMDARRAY;

/* ラベル配列 */
typedef struct {
    char *prog;
    char *label;
    WORD adr;
} LABELARRAY;

/* ラベル表 */
typedef struct _LABELTAB {
    struct _LABELTAB *next;
    char *prog;
    char *label;
    WORD adr;
} LABELTAB;

enum {
    LABELTABSIZE = 251,    /* ラベル表のサイズ */
};

/* アセンブラが、1回目か、2回目か、を表す */
typedef enum {
    FIRST = 0,
    SECOND = 1,
} PASS;

/* アセンブルのエラー定義 */
bool addcerrlist_assemble();

/* プログラム名とラベルに対応するハッシュ値を返す */
unsigned labelhash(const char *prog, const char *label);

/* プログラム名とラベルに対応するアドレスをラベル表から検索する */
WORD getlabel(const char *prog, const char *label);

/* ラベルを表に追加する */
bool addlabel(const char *prog, const char *label, WORD word);

/* ラベル表を表示する */
void printlabel();

/* ラベル表を解放する */
void freelabel();

/* オペランド */
typedef struct {
    int opdc;
    char *opdv[OPDSIZE];
} OPD;

/* 命令行 */
typedef struct {
    char *label;
    char *cmd;
    OPD *opd;
} CMDLINE;

/* 命令がマクロ命令の場合はメモリに書込
   書込に成功した場合TRUE、それ以外の場合はFALSEを返す */
bool macrocmd(const CMDLINE *cmdl, PASS pass);

/* 「,」区切りの文字列から、オペランドを取得する */
OPD *opdtok(const char *str);

/* 1行を解析する */
CMDLINE *linetok(const char *line);

/* 汎用レジスタを表す文字列「GR[0-7]」から、レジスタ番号[0-7]をWORD値で返す */
/* 文字列が汎用レジスタを表さない場合は、0xFFFFを返す */
/* is_xがtrueの場合は指標レジスタ。GR0は、COMET IIの仕様により、エラー発生 */
WORD getgr(const char *str, bool is_x);

/* 10進定数をWORDに変換 */
WORD getint(const char *str);

/* 16進定数をWORDに変換 */
WORD gethex(const char *str);

/* アドレスを返す */
/* アドレスには、リテラル／10進定数／16進定数／アドレス定数が含まれる */
WORD getadr(const char *prog, const char *str, PASS pass);

/* 定数の前に等号（=）をつけて記述される、リテラルを返す */
/* リテラルには、10進定数／16進定数／文字定数が含まれる */
WORD getliteral(const char *str, PASS pass);

/* COMET IIのメモリにアドレス値を書き込む */
bool writememory(WORD word, WORD adr, PASS pass);

/* ' 'で囲まれた、文字定数をメモリに書込 */
/* DC命令とリテラルで使い、リテラルの場合はリテラル領域に書込 */
void writestr(const char *str, bool literal, PASS pass);

/* アセンブラ命令DCをメモリに書込 */
void writeDC(const char *str, PASS pass);

/* 命令がアセンブラ命令の場合は処理を実行する。
   実行に成功した場合TRUE、それ以外の場合はFALSEを返す */
bool assemblecmd(const CMDLINE *cmdl, PASS pass);

/* 命令のコードを返す
   命令が無効な場合は0xFFFF */
WORD getcmd(CMDTYPE type, const char *cmd);

/* 命令が機械語命令の場合は処理を実行
   実行に成功した場合TRUE、それ以外の場合はFALSEを返す */
bool cometcmd(const CMDLINE *cmdl, PASS pass);

/* 1行のアセンブル */
/* アセンブル完了時はTRUE、未完了時はFALSEを返す */
bool assembleline(const CMDLINE *cmdl, PASS pass);

/* 指定された名前のファイルをアセンブル */
/* 2回実行される */
bool assemble(const char *file, PASS pass);

/* マクロ命令「IN IBUF,LEN」をメモリに書込 */
bool writeIN(const char *ibuf, const char *len, PASS pass);

/* マクロ命令「OUT OBUF,LEN」をメモリに書込 */
bool writeOUT(const char *obuf, const char *len, PASS pass);

/* マクロ命令「RPUSH」をメモリに書込 */
bool writeRPUSH(PASS pass);

/* マクロ命令「RPOP」をメモリに書込 */
bool writeRPOP(PASS pass);

#endif            /* YACASL2_ASSEMBLE_INCLUDEDの終端 */
