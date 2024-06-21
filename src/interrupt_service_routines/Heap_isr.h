#ifndef ISR80H_HEAP_H
#define ISR80H_HEAP_H

// storage symbol decleration
struct InterruptFrame;

extern void *isr80h_command4_malloc(struct InterruptFrame *frame);

extern void *isr80h_command5_free(struct InterruptFrame *frame);

#endif