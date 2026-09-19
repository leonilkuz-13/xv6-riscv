#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        printf("Usage: sumtest <a> <b>\n");
        exit(1);
    }

    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int result = sum(a,b);

    printf("Result: %d + %d = %d\n",a,b,result);

    exit(0);
}