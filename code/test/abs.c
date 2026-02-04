/* abs.c
 *	Simple program to test whether the systemcall interface works.
 *
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main() {
    int x = -10;
    int y;
    y = Abs(x);
    PrintNum(y); // Assuming printNum is a syscall to print a number
    return 0;
}
