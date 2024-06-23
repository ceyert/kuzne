/**
 * @file Task.h
 * @brief Task management for multitasking.
 *
 * Provides the definitions and functions necessary for creating, managing, and switching between
 * tasks in a multitasking environment. A task represents an individual unit of work, such as a process
 * or thread, within the system.
 */

#ifndef TASK_H
#define TASK_H

#include "Config.h"
#include "paging/Paging.h"

struct InterruptFrame; ///< Forward declaration for interrupt frame structure.
struct Process; ///< Forward declaration for process structure.

/**
 * @struct Registers
 * @brief CPU register context for a task.
 *
 * Represents the state of various general-purpose registers of the CPU
 * at a given point in time. Used for context switching between tasks.
 */
struct Registers {
    uint32_t edi, esi, ebp, ebx, edx, ecx, eax; ///< General purpose registers.
    uint32_t ip, cs, flags, esp, ss; ///< Instruction pointer, code segment, flags, stack pointer, stack segment.
};

/**
 * @struct Task
 * @brief Represents a single task in the system.
 *
 * Contains information necessary for task switching, including CPU register state,
 * page directory for memory management, and linkage to other tasks in a doubly linked list.
 */
struct Task {
    struct PageDirectory *page_directory; ///< Page directory for task's virtual memory.
    struct Registers registers; ///< Saved register state for the task.
    struct Process *process; ///< Associated process.
    struct Task *next; ///< Next task in the task list.
    struct Task *prev; ///< Previous task in the task list.
};


extern int task_init(struct Task* task, struct Process* process);

extern struct Task *task_new(struct Process *process);

extern struct Task *task_current();

extern struct Task *task_get_next();

extern int task_free(struct Task *task);

extern int set_current_task(struct Task *task);

extern int task_page();

extern int task_page_task(struct Task *task);

extern void run_first_task();

extern void task_return(struct Registers *regs);

extern void restore_general_purpose_registers(struct Registers *regs);

extern void user_registers();

extern void task_current_save_state(struct InterruptFrame *frame);

extern int copy_string_from_task(struct Task *task, void *virtual, void *phys, int max);

extern void *task_get_stack_item(struct Task *task, int index);

extern void *task_virtual_address_to_physical(struct Task *task, void *virtual_address);

extern void run_next_task();

#endif // TASK_H