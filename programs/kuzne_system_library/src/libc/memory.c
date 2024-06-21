#include "../../include/memory.h"

void* memset(void* ptr, int c, size_t size)
{
    char* c_ptr = (char*)ptr;
    for (int i = 0; i < size; i++)
    {
        c_ptr[i] = (char)c;
    }
    return ptr;
}

int memcmp(void* s1, void* s2, int count)
{
    char* c1 = s1;
    char* c2 = s2;
    while (count-- > 0)
    {
        if (*c1++ != *c2++)
        {
            return c1[-1] < c2[-1] ? -1 : 1;
        }
    }

    return 0;
}

void* memcpy(void* dest, void* src, int len)
{
    char* d = dest;
    char* s = src;
    while (len--)
    {
        *d++ = *s++;
    }
    return dest;
}

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
    while (ptr[i] != '\0')
    {
        i++;
    }

    return i;
}

int strnlen(const char* ptr, int max)
{
    int i = 0;
    for (i = 0; i < max; i++)
    {
        if (ptr[i] == '\0') break;
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

int istrncmp(const char* str1, const char* str2, int maxLength)
{
    unsigned char charFromStr1, charFromStr2;
    while (maxLength-- > 0)
    {
        charFromStr1 = (unsigned char)*str1++;  // Read and advance the pointer for str1
        charFromStr2 = (unsigned char)*str2++;  // Read and advance the pointer for str2
        if (charFromStr1 != charFromStr2 && tolower(charFromStr1) != tolower(charFromStr2))
            return charFromStr1 - charFromStr2;  // Return difference if characters don't match
        if (charFromStr1 == '\0')
        {
            return 0;  // Null character means end of string
        }
    }

    return 0;  // Return 0 if strings are equivalent up to maxLength characters
}

int strncmp(const char* str1, const char* str2, int maxLength)
{
    unsigned char charFromStr1, charFromStr2;

    while (maxLength-- > 0)
    {
        charFromStr1 = (unsigned char)*str1++;  // Read and advance the pointer for str1
        charFromStr2 = (unsigned char)*str2++;  // Read and advance the pointer for str2
        if (charFromStr1 != charFromStr2)
        {
            return charFromStr1 - charFromStr2;  // Return difference if characters don't match
        }

        if (charFromStr1 == '\0')
        {
            return 0;  // Null character means end of string
        }
    }

    return 0;  // Return 0 if strings are equivalent up to maxLength characters
}

char* strcpy(char* dest, const char* src)
{
    char* res = dest;
    while (*src != '\0')
    {
        *dest++ = *src++;
    }
    *dest = '\0';
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
    return c >= '0' && c <= '9';
}

int tonumericdigit(char c)
{
    return c - '0';
}

char* strtok(char* str, const char* delimiters)
{
    static char* sp = 0;
    int i = 0;
    int len = strlen(delimiters);
    if (!str && !sp) return 0;

    if (str && !sp)
    {
        sp = str;
    }

    char* p_start = sp;
    while (1)
    {
        for (i = 0; i < len; i++)
        {
            if (*p_start == delimiters[i])
            {
                p_start++;
                break;
            }
        }

        if (i == len)
        {
            sp = p_start;
            break;
        }
    }

    if (*sp == '\0')
    {
        sp = 0;
        return sp;
    }

    // Find end of substring
    while (*sp != '\0')
    {
        for (i = 0; i < len; i++)
        {
            if (*sp == delimiters[i])
            {
                *sp = '\0';
                break;
            }
        }

        sp++;
        if (i < len) break;
    }

    return p_start;
}

char* substr(const char* src, int start, int length, char* dest)
{
    if (start < 0 || length < 0 || start + length > strlen(src))
    {
        return 0x0;
    }
    for (int i = 0; i < length; i++)
    {
        dest[i] = src[start + i];
    }
    dest[length] = '\0';
    return dest;
}