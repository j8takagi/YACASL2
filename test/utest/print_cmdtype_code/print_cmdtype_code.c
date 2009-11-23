#include "casl2.h"
int main(){
    puts("== CMDTYPE_CODE TABLE ==");
    create_cmdtype_code();
    print_cmdtype_code();
    free_cmdtype_code();
    if(cerrno != 0) {
        printf("\terror - %d: %s\n", cerrno, cerrmsg);
        freecerr();
        exit(-1);
    }
    return 0;
}
