#ifndef ISR80H_MISC_H
#define ISR80H_MISC_H

// storage symbol decleration
struct InterruptFrame;

extern void *isr80h_command0_sum(struct InterruptFrame *frame);

#endif