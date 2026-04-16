/* tlb_test.c
 *
 * TLB stress test for Nachos.
 *
 * Goals:
 *   1. Demand paging: touch many virtual pages cold so every first access
 *      generates a TLB miss (PageFaultException) that HandleTLBMiss() must
 *      service.
 *   2. TLB replacement: stride across more than TLBSize (4) distinct pages
 *      in a single tight loop, forcing Round-Robin evictions.
 *   3. Correctness under replacement: write a pattern, evict those TLB
 *      entries by touching other pages, then read back and verify — confirms
 *      that evicted entries are reloaded correctly and that dirty bits are
 *      preserved.
 *   4. Re-use: a second sequential scan verifies that already-loaded pages
 *      can be re-entered into the TLB without corrupting data.
 *
 * PageSize = 128 bytes, TLBSize = 4.
 * An int is 4 bytes, so 128/4 = 32 ints per page.
 * STRIDE = 32 means every array access lands on a *different* page,
 * guaranteeing a TLB replacement after the 4th access in phase 2.
 */

#include "syscall.h"

/* ── configuration ─────────────────────────────────────────────────── */
#define INTS_PER_PAGE  32       /* PageSize(128) / sizeof(int)(4)       */
#define NUM_PAGES      10       /* touch 10 distinct pages of data      */
#define ARRAY_SIZE     (INTS_PER_PAGE * NUM_PAGES)  /* 320 ints         */

/* ── data in BSS (uninit) so it lives on separate virtual pages ─────── */
int A[ARRAY_SIZE];

/* ── helper ─────────────────────────────────────────────────────────── */
static void print_result(int pass, int which) {
    /* print "PASS n" or "FAIL n" using only PrintNum / PrintChar */
    if (pass) {
        PrintChar('P'); PrintChar('A'); PrintChar('S'); PrintChar('S');
    } else {
        PrintChar('F'); PrintChar('A'); PrintChar('I'); PrintChar('L');
    }
    PrintChar(' ');
    PrintNum(which);
    PrintChar('\n');
}

int main() {
    int i, ok;

    /* ── Phase 1: sequential write across all pages ─────────────────
     * Each group of 32 consecutive writes lives on one page.
     * 10 pages → 10 cold TLB misses minimum, plus replacements after
     * the 4th miss because TLBSize == 4.                              */
    for (i = 0; i < ARRAY_SIZE; i++) {
        A[i] = i;
    }

    /* ── Phase 2: strided read (one int per page per pass) ──────────
     * Access pattern: A[0], A[32], A[64], …, A[288]  (page 0..9)
     * then A[1], A[33], …  Every group of 4 accesses exhausts the TLB
     * and the 5th forces a Round-Robin eviction.                      */
    ok = 1;
    for (i = 0; i < INTS_PER_PAGE; i++) {
        int p;
        for (p = 0; p < NUM_PAGES; p++) {
            int idx = p * INTS_PER_PAGE + i;
            if (A[idx] != idx) {
                ok = 0;
            }
        }
    }
    print_result(ok, 1);   /* "PASS 1" if strided read matches written values */

    /* ── Phase 3: overwrite with inverse values ──────────────────────
     * Write A[i] = ARRAY_SIZE - 1 - i (reverse), then read back.
     * This exercises dirty-bit tracking: modified pages must not lose
     * their data when TLB entries are evicted and reloaded.           */
    for (i = 0; i < ARRAY_SIZE; i++) {
        A[i] = ARRAY_SIZE - 1 - i;
    }
    ok = 1;
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (A[i] != ARRAY_SIZE - 1 - i) {
            ok = 0;
        }
    }
    print_result(ok, 2);   /* "PASS 2" if reverse values are intact */

    /* ── Phase 4: partial re-use ─────────────────────────────────────
     * Write only the first half, read back the entire array.
     * Second half must still hold its phase-3 values (dirty tracking). */
    for (i = 0; i < ARRAY_SIZE / 2; i++) {
        A[i] = i * 2;          /* even numbers in first half */
    }
    ok = 1;
    for (i = 0; i < ARRAY_SIZE / 2; i++) {
        if (A[i] != i * 2) ok = 0;
    }
    for (i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++) {
        if (A[i] != ARRAY_SIZE - 1 - i) ok = 0;
    }
    print_result(ok, 3);   /* "PASS 3" if both halves are correct */

    Halt();
}
