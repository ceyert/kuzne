#ifndef KUZNE_STDIO
#define KUZNE_STDIO

extern void putchar(int character);

void print(const char *c_string);

extern void println(const char *c_string);

extern void printf(const char *fmt, ...);

#endif