/**
 * @file heap.h
 * @brief Definitions and functions for managing a dynamic memory heap.
 * 
 * This header file provides structures and functions for creating and managing 
 * a heap, including memory allocation and deallocation.
 */

#ifndef HEAP_H
#define HEAP_H

#include "Memory_Constants.h"
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Flag indicating that a heap table entry is occupied.
 */
#define HEAP_MAP_ENTRY_TAKEN 0x01

/**
 * @brief Flag indicating that a heap table entry is free.
 */
#define HEAP_MAP_ENTRY_FREE 0x00

/**
 * @brief Bitmask indicating that a heap table entry has a subsequent entry.
 */
#define HEAP_MAP_ENTRY_HAS_NEXT 0b10000000

/**
 * @brief Bitmask indicating that a heap table entry is the first in a block.
 */
#define HEAP_MAP_ENTRY_IS_FIRST 0b01000000

/**
 * @brief Structure representing a heap map.
 * 
 * The heap map keeps track of the usage state of heap memory through entries.
 */
struct HeapMap {
    unsigned char *mapBaseAddress_; /**< Base address of the heap map. */
    size_t totalEntries_;           /**< Total number of entries in the heap map. */
};

/**
 * @brief Structure representing a heap.
 * 
 * A heap includes a base address for the memory it manages and a map for tracking allocations.
 */
struct Heap {
    struct HeapMap *heapMap_; /**< Pointer to the heap map structure. */
    void *heapBaseAddr_;      /**< Base address of the heap memory region. */
};

/**
 * @brief Initializes a heap structure.
 * 
 * This function creates a heap within a specified memory region, using a provided heap map for tracking allocations.
 * 
 * @param heap Pointer to the `Heap` structure to initialize.
 * @param ptr Pointer to the start of the memory region for the heap.
 * @param end Pointer to the end of the memory region for the heap.
 * @param table Pointer to the `HeapMap` structure for tracking allocations.
 * @return 0 on success, or an error code on failure.
 */
extern int heap_create(struct Heap *heap, void *ptr, void *end, struct HeapMap *table);

/**
 * @brief Allocates a block of memory from the heap.
 * 
 * @param heap Pointer to the `Heap` structure to allocate from.
 * @param size Size of the memory block to allocate, in bytes.
 * @return Pointer to the allocated memory block, or NULL if allocation fails.
 */
extern void *heap_malloc(struct Heap *heap, size_t size);

/**
 * @brief Frees a previously allocated block of memory.
 * 
 * This function marks a memory block as free in the heap map, making it available for future allocations.
 * 
 * @param heap Pointer to the `Heap` structure to free memory from.
 * @param ptr Pointer to the memory block to free.
 */
extern void heap_free(struct Heap *heap, void *ptr);

#endif /* HEAP_H */