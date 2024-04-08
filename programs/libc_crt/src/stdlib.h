#ifndef KUZNE_STDLIB_H
#define KUZNE_STDLIB_H

#include <stddef.h>

void *malloc(size_t size);

void free(void *ptr);

char *itoa(int i);

char *itoa_hex(int i);

char *ptr_to_hex(const unsigned long ptr);

#endif