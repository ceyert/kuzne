#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <stddef.h>

extern void kheap_init();

extern void *kmalloc(size_t size);

extern void *kzalloc(size_t size);

extern void kfree(void *ptr);

#endif