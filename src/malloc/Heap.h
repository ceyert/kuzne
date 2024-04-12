#ifndef HEAP_H
#define HEAP_H

#include "Memory_Constants.h"
#include <stdint.h>
#include <stddef.h>


// Bit pipes of heap table entry
#define HEAP_MAP_ENTRY_TAKEN 0x01
#define HEAP_MAP_ENTRY_FREE 0x00

// Bit Masks of heap table entry
#define HEAP_MAP_ENTRY_HAS_NEXT 0b10000000
#define HEAP_MAP_ENTRY_IS_FIRST 0b01000000


struct HeapMap {
    unsigned char *mapBaseAddress_;
    size_t totalEntries_;
};


struct Heap {
    struct HeapMap *heapMap_;
    void *heapBaseAddr_;
};

extern int heap_create(struct Heap *heap, void *ptr, void *end, struct HeapMap *table);

extern void *heap_malloc(struct Heap *heap, size_t size);

extern void heap_free(struct Heap *heap, void *ptr);

#endif