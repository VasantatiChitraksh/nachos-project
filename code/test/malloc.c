#include "syscall.h"
#include "malloc.h"

static struct blk_metadata* heapStart = NULL;
static struct blk_metadata* heapTop   = NULL;

void* malloc(unsigned int reqSize) {
    struct blk_metadata* blk = NULL;
    struct blk_metadata* it  = heapStart;

    if (reqSize == 0) {
        return NULL;
    }

    /* Search free list for a block large enough (first-fit). */
    while (it != NULL) {
        if (it->in_use == 0 && it->size >= reqSize) {
            it->in_use = 1;
            return (void*)(it + 1);
        }
        it = it->next_blk;
    }

    /* No free block found — grow the heap. */
    blk = (struct blk_metadata*)Sbrk(reqSize + sizeof(struct blk_metadata));
    if ((int)blk == -1) {
        PrintString("Sbrk failed\n");
        return NULL;
    }

    blk->size     = reqSize;
    blk->in_use   = 1;
    blk->next_blk = NULL;
    blk->prev_blk = heapTop;

    if (heapTop != NULL) {
        heapTop->next_blk = blk;
    }
    heapTop = blk;

    if (heapStart == NULL) {
        heapStart = blk;
    }

    return (void*)(blk + 1);
}

void free(void* ptr) {
    struct blk_metadata* blk;

    if (ptr == NULL) {
        PrintString("NULL pointer passed to free\n");
        return;
    }

    blk = (struct blk_metadata*)ptr - 1;
    blk->in_use = 0;
}
