#include "../../include/stdio.h"
#include "../../include/stdlib.h"
#include "../../include/syscalls.h"
#include <stdarg.h>

void putchar(int c)
{
    kuzne_syscall_putchar((char)c);
}

void print(const char* c_string)
{
    kuzne_syscall_print(c_string);
}

void println(const char* c_string)
{
    kuzne_syscall_print(c_string);
    putchar('\n');
}

void printf(const char* fmt, ...)
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
                kuzne_syscall_print(itoa(ival));
                break;

            case 'p':
                pval = va_arg(ap, unsigned long);
                kuzne_syscall_print(ptr_to_hex(pval));
                break;

            case 'x':
                ival = va_arg(ap, int);
                kuzne_syscall_print(itoa_hex(ival));
                break;

            case 's':
                sval = va_arg(ap, char*);
                kuzne_syscall_print(sval);
                break;

            default:
                putchar(*p);
                break;
        }
    }

    va_end(ap);
}