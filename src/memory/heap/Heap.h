#ifndef HEAP_H
#define HEAP_H

#include "Config.h"
#include <stdint.h>
#include <stddef.h>

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST  0b01000000


typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct HeapTable {
    HEAP_BLOCK_TABLE_ENTRY *entries;
    size_t total;
};


struct Heap {
    struct HeapTable *table;

    // Start address of the heap data pool
    void *saddr;
};

int heap_create(struct Heap *heap, void *ptr, void *end, struct HeapTable *table);

void *heap_malloc(struct Heap *heap, size_t size);

void heap_free(struct Heap *heap, void *ptr);

#endif