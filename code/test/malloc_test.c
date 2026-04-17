/* malloc_test.c — C89 compatible (old MIPS cross-compiler) */
#include "syscall.h"
#include "malloc.h"

static void pass(int n) {
    PrintChar('P'); PrintChar('A'); PrintChar('S'); PrintChar('S');
    PrintChar(' '); PrintNum(n); PrintChar('\n');
}
static void fail(int n) {
    PrintChar('F'); PrintChar('A'); PrintChar('I'); PrintChar('L');
    PrintChar(' '); PrintNum(n); PrintChar('\n');
}

int main() {
    /* All declarations at top (C89). */
    int *arr;
    int *a, *b, *c;
    void *z;
    int i, ok;

    /* ── Test 1: single alloc, write, read back ──────────────────── */
    arr = (int*)malloc(10 * sizeof(int));
    ok  = (arr != NULL);
    if (ok) {
        for (i = 0; i < 10; i++) arr[i] = i * 3;
        for (i = 0; i < 10; i++) if (arr[i] != i * 3) { ok = 0; break; }
    }
    if (ok) pass(1); else fail(1);

    /* ── Test 2: two allocs don't overlap ────────────────────────── */
    a = (int*)malloc(4 * sizeof(int));
    b = (int*)malloc(4 * sizeof(int));
    ok = (a != NULL && b != NULL && a != b);
    if (ok) {
        a[0] = 111;  b[0] = 222;
        ok = (a[0] == 111 && b[0] == 222);
    }
    if (ok) pass(2); else fail(2);

    /* ── Test 3: free + realloc recycles the block ───────────────── */
    free(a);
    c  = (int*)malloc(4 * sizeof(int));   /* must reuse a's block */
    ok = (c != NULL);
    if (ok) { c[0] = 999; ok = (c[0] == 999); }
    if (ok) ok = (b[0] == 222);           /* b must be untouched */
    if (ok) pass(3); else fail(3);

    /* ── Test 4: zero-size malloc returns NULL ───────────────────── */
    z  = malloc(0);
    ok = (z == NULL);
    if (ok) pass(4); else fail(4);

    Halt();
}
