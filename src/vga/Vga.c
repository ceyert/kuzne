#include "Vga.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "string/String.h"


uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;
bool isOverride = false;


uint16_t terminal_make_char(char c, char colour) {
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour) {
    video_mem[(y * VGA_COL) + x] = terminal_make_char(c, colour);
}

void terminal_backspace() {
    if (terminal_row == 0 && terminal_col == 0) {
        return;
    }

    if (terminal_col == 0) {
        terminal_row -= 1;
        terminal_col = VGA_COL;
    }

    terminal_col -= 1;
    terminal_writechar(' ', 15);
    terminal_col -= 1;
}

static void cleanScreen(int col, int row)
{
    for (int y = row; y < VGA_ROW; y++) {
        for (int x = col; x < VGA_COL; x++) {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

void terminal_writechar(char c, char colour) {
    if (isOverride) {
        cleanScreen(terminal_col, terminal_row);
        isOverride = false;
    }

    if (c == '\n') {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }

    if (c == 0x08) {
        terminal_backspace();
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, colour);

    terminal_col += 1;

    // check new line
    if (terminal_col >= VGA_COL) {
        terminal_col = 0;
        terminal_row += 1;
    }

    // check end of screen
    if (terminal_row >= VGA_ROW) {
        terminal_row = 1;
        terminal_col = 0;
        isOverride = true;
    }
}

void terminal_initialize() {
    video_mem = (uint16_t * )(VGA_BASE_ADDR);

    terminal_row = 0;
    terminal_col = 0;
    isOverride = false;

    cleanScreen(terminal_col, terminal_row);
}


void print(const char *str) {
    size_t len = strlen(str);
    for (int i = 0; i < len; i++) {
        terminal_writechar(str[i], 15);
    }
}

void panic(const char *msg) {
    print(msg);
    while (1) {}
}

void log(const char *msg) {
    print(msg);
    terminal_writechar('\n', 15);
}

void logAddress(const char *msg, const unsigned long addr) {
    print(msg);
    print(ptr_to_hex(addr));
    terminal_writechar('\n', 15);
}