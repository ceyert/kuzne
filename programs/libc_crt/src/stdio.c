#include "stdio.h"
#include "stdlib.h"
#include "syscalls.h"
#include <stdarg.h>

int putchar(int c)
{
    kuzne_syscall_putchar((char)c);
    return 0;
}

int printf(const char* fmt, ...)
{
    va_list ap;
    const char* p;

    char* sval;
    int ival;
    unsigned long pval;

    va_start(ap, fmt);

    for (p = fmt; *p; p++)
    {
        if (*p != '%')
        {
            putchar(*p);
            continue;
        }

        switch (*++p)
        {
            case 'i':
                ival = va_arg(ap, int);
                print(itoa(ival));
                break;

            case 'p':
                pval = va_arg(ap, unsigned long);
                print(ptr_to_hex(pval));
                break;

            case 'x':
                ival = va_arg(ap, int);
                print(itoa_hex(ival));
                break;

            case 's':
                sval = va_arg(ap, char*);
                print(sval);
                break;

            default:
                putchar(*p);
                break;
        }
    }

    va_end(ap);

    return 0;
}