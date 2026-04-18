#ifndef MALLOC_H
#define MALLOC_H

#ifndef NULL
#define NULL ((void*)0)
#endif

struct blk_metadata {
    struct blk_metadata* prev_blk;
    struct blk_metadata* next_blk;
    unsigned int size;
    int in_use;
};

void* malloc(unsigned int reqSize);
void  free(void* ptr);

#endif /* MALLOC_H */
