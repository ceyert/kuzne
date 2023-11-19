#ifndef ISR80H_IO_H
#define ISR80H_IO_H

struct InterruptFrame;

extern void *isr80h_command1_print(struct InterruptFrame *frame);

extern void *isr80h_command2_getkey(struct InterruptFrame *frame);

extern void *isr80h_command3_putchar(struct InterruptFrame *frame);

#endif