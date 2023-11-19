#include "Heap.h"
#include "Kernel.h"
#include "status.h"
#include "memory/Memory.h"
#include <stdbool.h>

static int heap_validate_table(void *ptr, void *end, struct HeapTable *table) {
    int res = 0;

    size_t table_size = (size_t)(end - ptr);
    size_t total_blocks = table_size / HEAP_BLOCK_SIZE;
    if (table->total != total_blocks) {
        res = -EINVARG;
        goto out;
    }

    out:
    return res;
}

static bool heap_validate_alignment(void *ptr) {
    return ((unsigned int) ptr % HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct Heap *heap, void *heapBaseAddr, void *end, struct HeapTable *table) {
    int res = 0;

    if (!heap_validate_alignment(heapBaseAddr) || !heap_validate_alignment(end)) {
        res = -EINVARG;
        goto out;
    }

    memset(heap, 0, sizeof(struct Heap));
    heap->heapBaseAddr = heapBaseAddr;
    heap->heapTable = table;

    res = heap_validate_table(heapBaseAddr, end, table);
    if (res < 0) {
        goto out;
    }

    size_t table_size = sizeof(heap_table_entry_t) * table->total;
    memset(table->tableEntries, HEAP_TABLE_ENTRY_FREE, table_size);

    out:
    return res;
}

static uint32_t heap_align_value_to_upper(uint32_t val) {
    if ((val % HEAP_BLOCK_SIZE) == 0) {
        return val;
    }

    val = (val - (val % HEAP_BLOCK_SIZE));
    val += HEAP_BLOCK_SIZE;
    return val;
}

static int heap_get_entry_type(heap_table_entry_t entry) {
    return entry & 0x0f;
}

int heap_get_start_block(struct Heap *heap, uint32_t total_blocks) {
    struct HeapTable *table = heap->heapTable;
    int bc = 0;
    int bs = -1;

    for (size_t i = 0; i < table->total; i++) {
        if (heap_get_entry_type(table->tableEntries[i]) != HEAP_TABLE_ENTRY_FREE) {
            bc = 0;
            bs = -1;
            continue;
        }

        // If this is the first block
        if (bs == -1) {
            bs = i;
        }
        bc++;
        if (bc == total_blocks) {
            break;
        }
    }

    if (bs == -1) {
        return -ENOMEM;
    }

    return bs;

}

void *heap_block_to_address(struct Heap *heap, int block) {
    return heap->heapBaseAddr + (block * HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_taken(struct Heap *heap, int start_block, int total_blocks) {
    int end_block = (start_block + total_blocks) - 1;

    heap_table_entry_t entry = HEAP_TABLE_ENTRY_TAKEN | HEAP_TABLE_ENTRY_IS_FIRST;
    if (total_blocks > 1) {
        entry |= HEAP_TABLE_ENTRY_HAS_NEXT;
    }

    for (int i = start_block; i <= end_block; i++) {
        heap->heapTable->tableEntries[i] = entry;
        entry = HEAP_TABLE_ENTRY_TAKEN;
        if (i != end_block - 1) {
            entry |= HEAP_TABLE_ENTRY_HAS_NEXT;
        }
    }
}

void *heap_malloc_blocks(struct Heap *heap, uint32_t total_blocks) {
    void *address = 0;

    int start_block = heap_get_start_block(heap, total_blocks);
    if (start_block < 0) {
        goto out;
    }

    address = heap_block_to_address(heap, start_block);

    // Mark the blocks as taken
    heap_mark_blocks_taken(heap, start_block, total_blocks);

    out:
    return address;
}

void heap_mark_blocks_free(struct Heap *heap, int starting_block) {
    struct HeapTable *table = heap->heapTable;
    for (int i = starting_block; i < (int) table->total; i++) {
        heap_table_entry_t entry = table->tableEntries[i];
        table->tableEntries[i] = HEAP_TABLE_ENTRY_FREE;
        if (!(entry & HEAP_TABLE_ENTRY_HAS_NEXT)) {
            break;
        }
    }
}

int heap_address_to_block(struct Heap *heap, void *address) {
    return ((int) (address - heap->heapBaseAddr)) / HEAP_BLOCK_SIZE;
}

void *heap_malloc(struct Heap *heap, size_t size) {
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / HEAP_BLOCK_SIZE;
    return heap_malloc_blocks(heap, total_blocks);
}

void heap_free(struct Heap *heap, void *ptr) {
    heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}