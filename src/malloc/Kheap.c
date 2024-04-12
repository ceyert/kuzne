#include "Config.h"
#include "Heap.h"
#include "Kernel.h"
#include "memory/Memory.h"
#include "vga/Vga.h"

struct Heap KERNEL_HEAP_;
struct HeapMap KERNEL_HEAP_MAP_;

void kernel_heap_init()
{
    KERNEL_HEAP_MAP_.mapBaseAddress_ = (unsigned char*)(HEAP_MAP_BASE_ADDRESS);
    KERNEL_HEAP_MAP_.totalEntries_ = TOTAL_HEAP_MAP_BLOCKS;

    void* end = (void*)(HEAP_BASE_ADDRESS + TOTAL_HEAP_SIZE);
    int res = heap_create(&KERNEL_HEAP_, (void*)(HEAP_BASE_ADDRESS), end, &KERNEL_HEAP_MAP_);
    if (res < 0)
    {
        panic("Error: kernel_heap_init");
    }
}

void* kernel_malloc(size_t size)
{
    return heap_malloc(&KERNEL_HEAP_, size);
}

void* kernel_zeroed_alloc(size_t size)
{
    void* ptr = kernel_malloc(size);
    if (!ptr) return 0;

    memset(ptr, 0x00, size);
    return ptr;
}

void kernel_free_alloc(void* ptr)
{
    heap_free(&KERNEL_HEAP_, ptr);
}