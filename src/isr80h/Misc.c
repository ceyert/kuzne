#include "Misc.h"
#include "idt/Idt.h"
#include "task/Task.h"

void *isr80h_command0_sum(struct InterruptFrame *frame) {
    int v2 = (int) task_get_stack_item(task_current(), 1);
    int v1 = (int) task_get_stack_item(task_current(), 0);
    return (void *) (v1 + v2);
}