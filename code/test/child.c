#include "syscall.h"
#define stdin 0
#define stdout 1

int main() {
    int i = 0;
    for (i = 0; i < 10; i++){
        Write("This is the child thread.\n", 27, stdout);
    }

    return 0;
}
