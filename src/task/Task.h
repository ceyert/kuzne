#ifndef TASK_H
#define TASK_H

#include "Config.h"
#include "memory/paging/Paging.h"

// storage symbol declerations
struct InterruptFrame;
struct Process;

struct Registers {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct Task {
    /**
     * The page directory of the task
     */
    struct Paging4GbChunk *page_directory;

    // The registers of the task when the task is not running
    struct Registers registers;

    // The process of the task
    struct Process *process;

    // The next task in the linked list
    struct Task *next;

    // Previous task in the linked list
    struct Task *prev;
};

extern struct Task *task_new(struct Process *process);

extern struct Task *task_current();

extern struct Task *task_get_next();

extern int task_free(struct Task *task);

extern int task_switch(struct Task *task);

extern int task_page();

extern int task_page_task(struct Task *task);

extern void task_run_first_ever_task();

extern void task_return(struct Registers *regs);

extern void restore_general_purpose_registers(struct Registers *regs);

extern void user_registers();

extern void task_current_save_state(struct InterruptFrame *frame);

extern int copy_string_from_task(struct Task *task, void * virtual,

void *phys,
int max
);

extern void *task_get_stack_item(struct Task *task, int index);

extern void *task_virtual_address_to_physical(struct Task *task, void *virtual_address);

extern void task_next();

#endif