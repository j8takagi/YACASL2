#include "cmem.h"

void *malloc_chk(size_t size, const char *tag)
{
    void *p;

    if((p = malloc(size)) == NULL) {
        fprintf(stderr, "%s: cannot allocate memory\n", tag);
        exit(1);
    }
    return memset(p, 0, size);
}

void *calloc_chk(size_t nmemb, size_t size, char *tag)
{
    void *p;

    if((p = calloc(nmemb, size)) == NULL) {
        fprintf(stderr, "%s: cannot allocate memory\n", tag);
        exit(1);
    }
    return p;
}

char *strdup_chk(const char *s, char *tag)
{
    assert(s != NULL);
    char *t;

    t = malloc_chk(strlen(s) + 1, tag);
    strcpy(t, s);
    return t;
}

char *strndup_chk(const char *s, size_t len, char *tag)
{
    assert(s != NULL);
    char *t;

    if(len < strlen(s)) {
        t = malloc_chk(len + 1, tag);
        strncpy(t, s, len);
        t[len] = '\0';
    } else {
        t = strdup_chk(s, tag);
    }
    return t;
}
