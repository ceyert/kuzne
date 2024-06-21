#include "Misc_isr.h"
#include "interrupt_descriptor_table/Idt.h"
#include "process/Task.h"

void *isr80h_command0_sum(struct InterruptFrame *frame) 
{
    int v2 = (int) task_get_stack_item(task_current(), 1);
    int v1 = (int) task_get_stack_item(task_current(), 0);
    return (void *) (v1 + v2);
}