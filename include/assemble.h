/* CASL IIの制限 */
enum {
    LINESIZE = 1024,    /* 行の最大文字数 */
    TOKENSIZE = 256,    /* トークンの最大文字数 */
    LABELSIZE = 8,    /* ラベルの最大文字数 */
    LABELTABSIZE = 256,    /* ラベルの最大数 */
    OPDSIZE = 40     /* オペラントの最大数 */
};

/* 値を格納するポインタ */
extern WORD ptr;

/* リテラル（=付きの値）を格納するポインタ */
extern WORD lptr;

/* アセンブラ命令とマクロ命令を表す番号 */
typedef enum {
    START = 01,
    END = 02,
    DS = 03,
    DC = 04,
    IN = 011,
    OUT = 012,
    RPUSH = 013,
    RPOP = 014
} CASLCMD;

/* アセンブラ命令とマクロ命令を表す配列 */
typedef struct {
    CASLCMD cmdid;
    int opdc_min;
    int opdc_max;
    char *cmd;
} CMDARRAY;

/* ラベル表 */
typedef struct _LABELTAB {
    struct _LABELTAB *next;
    char *name;
    WORD adr;
} LABELTAB;

/* アセンブラが、1回目か、2回目か、を表す */
typedef enum {
    FIRST = 0,
    SECOND = 1
} PASS;

/* ラベル表からアドレスを検索する */
WORD getlabel(const char *label);

/* ラベルを表に追加する */
bool addlabel(const char *label, WORD word);

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

/* 汎用レジスタを表す文字列「GR[0-7]」をWORD値に変換
   is_xがTRUEの場合は、指標レジスタとして用いる汎用レジスタ
   文字列が汎用レジスタを表さない場合は、0xFFFFを返す */
WORD getgr(const char *str, bool is_x);

/* 10進定数をWORDに変換 */
WORD getint(const char *str);

/* 16進定数をWORDに変換 */
WORD gethex(const char *str);

/* アドレスを返す */
/* アドレスには、リテラル／10進定数／16進定数／アドレス定数が含まれる */
WORD getadr(const char *str, PASS pass);

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
