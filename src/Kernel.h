#ifndef KERNEL_H
#define KERNEL_H

#define PEACHOS_MAX_PATH 108

extern void kernel_main();

extern void kernel_page();

extern void kernel_registers();

#define ERROR(value) (void*)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

#endif