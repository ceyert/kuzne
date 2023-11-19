#include "stdlib.h"
#include "peachos.h"

char *itoa(int i) {
    static char text[12];
    int loc = 11;
    text[11] = 0;
    char neg = 1;
    if (i >= 0) {
        neg = 0;
        i = -i;
    }

    while (i) {
        text[--loc] = '0' - (i % 10);
        i /= 10;
    }

    if (loc == 11)
        text[--loc] = '0';

    if (neg)
        text[--loc] = '-';

    return &text[loc];
}

char *itoa_hex(int i) {
    static char text[9]; // Buffer for 8 hex digits + null terminator. Note: This gets overwritten on each call.
    int loc = 8;
    text[8] = '\0'; // Null-terminator for string

    if (i == 0) {
        text[7] = '0'; // Handle 0 explicitly
        return &text[7];
    }

    // Convert to hexadecimal
    while (i != 0 && loc > 0) {
        int digit = i & 0xF; // Extract the last 4 bits
        text[--loc] = (digit > 9) ? (digit - 10 + 'A') : (digit + '0');
        i >>= 4; // Shift right by 4 bits to process the next digit
    }

    return &text[loc];
}

char *ptr_to_hex(const unsigned long ptr) {
    static char text[17]; // Buffer for 16 hex digits (for 64-bit addresses) + null terminator
    static const char hexDigits[] = "0123456789abcdef";
    unsigned long addr = ptr; // Cast pointer to unsigned long
    int loc = 16;
    text[16] = '\0'; // Null-terminator for string

    for (int i = 0; i < 16; ++i) {
        text[i] = '0'; // Initialize buffer with zeros
    }

    if (addr == 0) {
        return &text[15]; // Return "0" for null pointer
    }

    // Convert to hexadecimal
    while (addr != 0 && loc > 0) {
        int digit = addr & 0xF; // Extract the last 4 bits
        text[--loc] = hexDigits[digit];
        addr >>= 4; // Shift right by 4 bits to process the next digit
    }

    return &text[loc];
}


void *malloc(size_t size) {
    return peachos_malloc(size);
}

void free(void *ptr) {
    peachos_free(ptr);
}