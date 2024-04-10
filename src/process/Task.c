#include "Kernel.h"
#include "Process.h"
#include "Status.h"
#include "Task.h"
#include "interrupt_descriptor_table/Idt.h"
#include "loader/Elfloader.h"
#include "memory/Memory.h"
#include "memory/heap/Kheap.h"
#include "memory/paging/Paging.h"
#include "string/String.h"
#include "vga/Vga.h"

// The current task that is running
struct Task* current_task = 0;

// Task linked list
struct Task* task_tail = 0;
struct Task* task_head = 0;

// Function to initialize a task with a given process
int task_init(struct Task* task, struct Process* process);

// Function to retrieve the currently running task
struct Task* task_current()
{
    return current_task;
}

// Function to create a new task for a given process
struct Task* task_new(struct Process* process)
{
    int res = 0;
    struct Task* task = kzalloc(sizeof(struct Task));
    if (!task)
    {
        res = -ENOMEM;
        goto out;
    }

    res = task_init(task, process);
    if (res != ALL_OK)
    {
        goto out;
    }

    if (task_head == 0)
    {
        task_head = task;
        task_tail = task;
        current_task = task;
        goto out;
    }

    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;

out:
    if (ISERR(res))
    {
        task_free(task);
        return ERROR(res);
    }

    return task;
}

// Function to get the next task in the linked list
struct Task* task_get_next()
{
    if (!current_task->next)
    {
        return task_head;
    }

    return current_task->next;
}

// Internal function to remove a task from the task list
static void task_list_remove(struct Task* task)
{
    if (task->prev)
    {
        task->prev->next = task->next;
    }

    if (task == task_head)
    {
        task_head = task->next;
    }

    if (task == task_tail)
    {
        task_tail = task->prev;
    }

    if (task == current_task)
    {
        current_task = task_get_next();
    }
}

// Function to free resources associated with a task
int task_free(struct Task* task)
{
    paging_free_4gb(task->page_directory);
    task_list_remove(task);

    // Finally free the task data
    kfree(task);
    return 0;
}

// Function to switch to the next task
void task_next()
{
    struct Task* next_task = task_get_next();
    if (!next_task)
    {
        panic("No more tasks!\n");
    }

    task_switch(next_task);
    task_return(&next_task->registers);
}

// Function to switch the current task to a specified task
int task_switch(struct Task* task)
{
    current_task = task;
    paging_switch(task->page_directory);
    return 0;
}

// Function to save the state of a task, typically used before switching tasks
void task_save_state(struct Task* task, struct InterruptFrame* frame)
{
    task->registers.ip = frame->ip;
    task->registers.cs = frame->cs;
    task->registers.flags = frame->flags;
    task->registers.esp = frame->esp;
    task->registers.ss = frame->ss;
    task->registers.eax = frame->eax;
    task->registers.ebp = frame->ebp;
    task->registers.ebx = frame->ebx;
    task->registers.ecx = frame->ecx;
    task->registers.edi = frame->edi;
    task->registers.edx = frame->edx;
    task->registers.esi = frame->esi;
}

// Function to copy a string from one task to another, handling virtual to physical address mapping
int copy_string_from_task(struct Task* task, void* virtual, void* phys, int max)
{
    if (max >= PAGE_SIZE)
    {
        return -EINVARG;
    }

    int res = 0;
    char* tmp = kzalloc(max);
    if (!tmp)
    {
        res = -ENOMEM;
        goto out;
    }

    uint32_t* task_directory = task->page_directory->directory_entry;
    uint32_t old_entry = paging_get(task_directory, tmp);
    map_virtual_address_to_physical_address(task->page_directory, tmp, tmp, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(task->page_directory);
    strncpy(tmp, virtual, max);
    kernel_page();

    res = paging_set(task_directory, tmp, old_entry);
    if (res < 0)
    {
        res = -EIO;
        goto out_free;
    }

    strncpy(phys, tmp, max);

out_free:
    kfree(tmp);
out:
    return res;
}

// Function to save the state of the current task
void task_current_save_state(struct InterruptFrame* frame)
{
    if (!task_current())
    {
        panic("No current task to save\n");
    }

    struct Task* task = task_current();
    task_save_state(task, frame);
}

// Function to switch the page directory to the current task's page directory
int task_page()
{
    user_registers();
    task_switch(current_task);
    return 0;
}

// Function to switch the page directory to a specified task's page directory
int task_page_task(struct Task* task)
{
    user_registers();
    paging_switch(task->page_directory);
    return 0;
}

// Function to start the first ever task in the system
void task_run_first_ever_task()
{
    if (!current_task)
    {
        panic("task_run_first_ever_task(): No current task exists!\n");
    }

    task_switch(task_head);
    task_return(&task_head->registers);
}

// Function to initialize a task, setting up its page directory and registers
int task_init(struct Task* task, struct Process* process)
{
    memset(task, 0, sizeof(struct Task));
    // Enable 4GB memory regions
    task->page_directory = enable_4gb_virtual_memory_addressing(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if (!task->page_directory)
    {
        return -EIO;
    }

    task->registers.ip = USER_PROCESS_VIRTUAL_BASE_ADDRESS_NON_ELF;
    
    if (process->fileType == PROCESS_FILETYPE_ELF)
    {
        task->registers.ip = elf_header(process->elfFile)->e_entry;
    }

    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.cs = USER_CODE_SEGMENT;
    task->registers.esp = USER_PROCESS_STACK_VIRTUAL_ADDRESS_END; // SP grows high to low

    task->process = process;

    logAddress("process IP: ", (unsigned long)task->registers.ip);
    logAddress("process esp: ", (unsigned long)task->registers.esp);

    return 0;
}

// Function to get a specific item from a task's stack
void* task_get_stack_item(struct Task* task, int index)
{
    void* result = 0;

    uint32_t* sp_ptr = (uint32_t*)task->registers.esp;

    // Switch to the given tasks page
    task_page_task(task);

    result = (void*)sp_ptr[index];

    // Switch back to the kernel page
    kernel_page();

    return result;
}

// Function to convert a virtual address in a task's address space to a physical address
void* task_virtual_address_to_physical(struct Task* task, void* virtual_address)
{
    return paging_get_physical_address(task->page_directory->directory_entry, virtual_address);
}