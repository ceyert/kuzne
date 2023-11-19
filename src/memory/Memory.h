#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

extern void *memset(void *ptr, int c, size_t size);

extern int memcmp(void *s1, void *s2, int count);

extern void *memcpy(void *dest, void *src, int len);

#endif