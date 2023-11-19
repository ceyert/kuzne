#include "terminal/Terminal.h"
#include "Heap.h"
#include "Config.h"
#include "Kernel.h"
#include "memory/Memory.h"

struct Heap kernel_heap;
struct HeapTable kernel_heap_table;

void kheap_init() {
    int total_table_entries = TOTAL_HEAP_SIZE / HEAP_BLOCK_SIZE;
    kernel_heap_table.tableEntries = (heap_table_entry_t *) (HEAP_TABLE_BASE_ADDRESS);
    kernel_heap_table.total = total_table_entries;

    void *end = (void *) (HEAP_BASE_ADDRESS + TOTAL_HEAP_SIZE);
    int res = heap_create(&kernel_heap, (void *) (HEAP_BASE_ADDRESS), end, &kernel_heap_table);
    if (res < 0) {
        print("Failed to create heap\n");
    }

}

void *kmalloc(size_t size) {
    return heap_malloc(&kernel_heap, size);
}

void *kzalloc(size_t size) {
    void *ptr = kmalloc(size);
    if (!ptr)
        return 0;

    memset(ptr, 0x00, size);
    return ptr;
}

void kfree(void *ptr) {
    heap_free(&kernel_heap, ptr);
}