#include "Heap_isr.h"
#include "process/Process.h"
#include "process/Task.h"
#include <stddef.h>

void* isr80h_command4_malloc(struct InterruptFrame* frame)
{
    size_t size = (int)task_get_stack_item(task_current(), 0);

    return process_malloc(task_current()->process, size);
}

void* isr80h_command5_free(struct InterruptFrame* frame)
{
    void* ptr_to_free = task_get_stack_item(task_current(), 0);

    process_free(task_current()->process, ptr_to_free);
    
    return 0;
}