#ifndef TERMINAL_H
#define TERMINAL_H

#define VGA_BASE_ADDR 0xB8000

#define VGA_COL 80
#define VGA_ROW 20

extern void terminal_initialize();

extern void print(const char *str);

extern void terminal_writechar(char c, char colour);

extern void panic(const char *msg);

extern void log(const char *msg);

extern void logAddress(const char *msg, const unsigned long addr);

#endif