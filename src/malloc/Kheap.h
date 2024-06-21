#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <stddef.h>

extern void kernel_heap_init();

extern void *kernel_malloc(size_t size);

extern void *kernel_zeroed_alloc(size_t size);

extern void kernel_free_alloc(void *ptr);

#endif