#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdbool.h>

extern void *memset(void *ptr, int c, size_t size);

extern int memcmp(void *s1, void *s2, int count);

extern void *memcpy(void *dest, void *src, int len);

extern int strlen(const char *ptr);

extern int strnlen(const char *ptr, int max);

extern bool isdigit(char c);

extern int tonumericdigit(char c);

extern char *strcpy(char *dest, const char *src);

extern char *strncpy(char *dest, const char *src, int count);

extern int strncmp(const char *str1, const char *str2, int n);

extern int istrncmp(const char *s1, const char *s2, int n);

extern int strnlen_terminator(const char *str, int max, char terminator);

extern char *strtok(char *str, const char *delimiters);

extern char* substr(const char* src, int start, int length, char* dest);

extern char tolower(char s1);

extern char *itoa(int i);

extern char *itoa_hex(int i);

extern char *ptr_to_hex(const unsigned long ptr);

#endif