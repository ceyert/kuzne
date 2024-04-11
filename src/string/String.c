#include "String.h"

char tolower(char s1)
{
    if (s1 >= 65 && s1 <= 90)
    {
        s1 += 32;
    }

    return s1;
}

int strlen(const char* ptr)
{
    int i = 0;
    while (*ptr != 0)
    {
        i++;
        ptr += 1;
    }

    return i;
}

int strnlen(const char* ptr, int max)
{
    int i = 0;
    for (i = 0; i < max; i++)
    {
        if (ptr[i] == 0) break;
    }

    return i;
}

int strnlen_terminator(const char* str, int max, char terminator)
{
    int i = 0;
    for (i = 0; i < max; i++)
    {
        if (str[i] == '\0' || str[i] == terminator) break;
    }

    return i;
}

int istrncmp(const char* s1, const char* s2, int n)
{
    unsigned char u1, u2;
    while (n-- > 0)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2 && tolower(u1) != tolower(u2)) return u1 - u2;
        if (u1 == '\0') return 0;
    }

    return 0;
}

int strncmp(const char* str1, const char* str2, int n)
{
    unsigned char u1, u2;

    while (n-- > 0)
    {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if (u1 != u2) return u1 - u2;
        if (u1 == '\0') return 0;
    }

    return 0;
}

char* strcpy(char* dest, const char* src)
{
    char* res = dest;
    while (*src != 0)
    {
        *dest = *src;
        src += 1;
        dest += 1;
    }

    *dest = 0x00;

    return res;
}

char* strncpy(char* dest, const char* src, int count)
{
    int i = 0;
    for (i = 0; i < count - 1; i++)
    {
        if (src[i] == 0x00) break;

        dest[i] = src[i];
    }

    dest[i] = 0x00;
    return dest;
}

bool isdigit(char c)
{
    return c >= 48 && c <= 57;
}

int tonumericdigit(char c)
{
    return c - 48;
}

char* itoa(int i)
{
    static char text[12];
    int loc = 11;
    text[11] = 0;
    char neg = 1;
    if (i >= 0)
    {
        neg = 0;
        i = -i;
    }

    while (i)
    {
        text[--loc] = '0' - (i % 10);
        i /= 10;
    }

    if (loc == 11) text[--loc] = '0';

    if (neg) text[--loc] = '-';

    return &text[loc];
}

char* itoa_hex(int i)
{
    static char text[9];  // Buffer for 8 hex digits + null terminator. Note: This gets overwritten on each call.
    int loc = 8;
    text[8] = '\0';  // Null-terminator for string

    if (i == 0)
    {
        text[7] = '0';  // Handle 0 explicitly
        return &text[7];
    }

    // Convert to hexadecimal
    while (i != 0 && loc > 0)
    {
        int digit = i & 0xF;  // Extract the last 4 bits
        text[--loc] = (digit > 9) ? (digit - 10 + 'A') : (digit + '0');
        i >>= 4;  // Shift right by 4 bits to process the next digit
    }

    return &text[loc];
}

char* ptr_to_hex(const unsigned long ptr)
{
    static char text[17];  // Buffer for 16 hex digits (for 64-bit addresses) + null terminator

    static const char hexDigits[] = "0123456789ABCDEF";
    
    unsigned long addr = ptr;  
    int loc = 16;

    text[loc] = '\0';  

    // Initialize buffer with zeros
    for (int i = 0; i < 16; ++i)
    {
        text[i] = '0';  
    }

    if (addr == 0)
    {
        return &text[15];  // Return "0" for null pointer
    }

    // Convert to hexadecimal
    while (addr != 0 && loc > 0)
    {
        int digit = addr & 0xF;  // Extract the last 4 bits
        text[--loc] = hexDigits[digit];
        addr >>= 4;  // Shift right by 4 bits to process the next digit
    }

    return &text[loc];
}