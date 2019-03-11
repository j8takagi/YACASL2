#include "cmem.h"

void *malloc_chk(size_t size, const char *tag)
{
    void *p = NULL;

    if((p = malloc(size)) == NULL) {
        fprintf(stderr, "%s: cannot allocate memory\n", tag);
        exit(1);
    }
    return memset(p, 0, size);
}

void *calloc_chk(size_t nmemb, size_t size, const char *tag)
{
    void *p = NULL;

    if((p = calloc(nmemb, size)) == NULL) {
        fprintf(stderr, "%s: cannot allocate memory\n", tag);
        exit(1);
    }
    return p;
}

char *strdup_chk(const char *s, const char *tag)
{
    assert(s != NULL);
    char *t = NULL;

    t = malloc_chk(strlen(s) + 1, tag);
    strcpy(t, s);
    return t;
}

char *strndup_chk(const char *s, size_t len, const char *tag)
{
    assert(s != NULL);
    char *t = NULL;

    if(len < strlen(s)) {
        t = malloc_chk(len + 1, tag);
        strncpy(t, s, len);
        t[len] = '\0';
    } else {
        t = strdup_chk(s, tag);
    }
    return t;
}

void strip_end(char *s)
{
    for(int i = strlen(s) - 1; i > 0 && (s[i] == '\n' || s[i] == '\r' || s[i] == ' ' || s[i] == '\t'); i--) {
        s[i] = '\0';
    }
}

void strip_casl2_comment(char *s)
{
    bool quoting = false;

    for(int i = 0; s[i]; i++) {
        /* 「'」で囲まれた文字列の場合。「''」は無視 */
        if(s[i] == '\'' && s[i+1] != '\'' && (quoting == false || s[i-1] != '\'')) {
            quoting = !quoting;
        /* 「'」で囲まれた文字列でない場合、文字列末尾の「;」以降を削除 */
        } else if(quoting == false && s[i] == ';') {
            s[i] = '\0';
            break;
        }
    }
}

}
