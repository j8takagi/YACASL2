#include "casl2.h"
int main(){
    puts("== CODE_TYPE TABLE ==");
    create_code_type();
    print_code_type();
    free_code_type();
    if(cerrno != 0) {
        printf("\terror - %d: %s\n", cerrno, cerrmsg);
        freecerr();
        exit(-1);
    }
    return 0;
}
