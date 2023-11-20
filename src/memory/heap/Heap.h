#ifndef HEAP_H
#define HEAP_H

#include "Config.h"
#include <stdint.h>
#include <stddef.h>

typedef unsigned char heap_table_entry_t;

// Bit pipes of heap table entry
#define HEAP_TABLE_ENTRY_TAKEN 0x01
#define HEAP_TABLE_ENTRY_FREE 0x00

// Bit Masks of heap table entry
#define HEAP_TABLE_ENTRY_HAS_NEXT 0b10000000
#define HEAP_TABLE_ENTRY_IS_FIRST 0b01000000


struct HeapTable {
    heap_table_entry_t *tableEntries;
    size_t totalEntries;
};


struct Heap {
    struct HeapTable *heapTable;

    // Base address of the heap!
    void *heapBaseAddr;
};

extern int heap_create(struct Heap *heap, void *ptr, void *end, struct HeapTable *table);

extern void *heap_malloc(struct Heap *heap, size_t size);

extern void heap_free(struct Heap *heap, void *ptr);

#endif