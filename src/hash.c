/* ハッシュ値を取得する */
unsigned hash(const char *key, int size){
    unsigned hashval;

    for(hashval = 0; *key != '\0'; key++){
        hashval = *key + 31 * hashval;
    }
    return hashval % size;
}
