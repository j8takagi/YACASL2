/* ハッシュ共用体の型 */
typedef enum {
    CHARS = 0,
    INT = 1,
} UTYPE;

/* ハッシュ共用体 */
typedef struct {
    UTYPE type;
    union {
        char *s;
        int i;
    } val;
} HKEY;

/* ハッシュ値を取得する */
unsigned hash(int keyc, HKEY *keyv[], int tabsize);
