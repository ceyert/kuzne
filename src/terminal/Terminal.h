#ifndef TERMINAL_H
#define TERMINAL_H

#define VGA_BASE_ADDR 0xB8000

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

void terminal_initialize();

void print(const char *str);

void terminal_writechar(char c, char colour);

void panic(const char *msg);

#endif