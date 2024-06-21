#include "Kernel.h"
#include "Process.h"
#include "Status.h"
#include "Task.h"
#include "interrupt_descriptor_table/Idt.h"
#include "loader/Elfloader.h"
#include "memory/Memory.h"
#include "malloc/Kheap.h"
#include "paging/Paging.h"
#include "vga/Vga.h"

struct Task* CURRENT_TASK_ = 0;

// Task doubly linked list
struct Task* TASK_FRONT_ = 0;
struct Task* TASK_TAIL_ = 0;

int task_init(struct Task* task, struct Process* process);


// Function to convert a virtual address in a task's address space to a physical address
void* task_virtual_address_to_physical(struct Task* task, void* virtual_address)
{
    return paging_get_physical_address(task->page_directory->directory_entry, virtual_address);
}


struct Task* task_current()
{
    return CURRENT_TASK_;
}

// get next task in linked list
struct Task* task_get_next()
{
    if (!CURRENT_TASK_->next)
    {
        return TASK_FRONT_;
    }

    return CURRENT_TASK_->next;
}


// Function to switch to the next task
void run_next_task()
{
    struct Task* next_task = task_get_next();
    if (!next_task)
    {
        panic("No more tasks!\n");
    }

    set_current_task(next_task);
    task_return(&next_task->registers);
}

// Function to switch the current task to a specified task
int set_current_task(struct Task* task)
{
    CURRENT_TASK_ = task;
    set_current_page_directory(task->page_directory);
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
    char* tmp = kernel_zeroed_alloc(max);
    if (!tmp)
    {
        res = -ENOMEM;
        goto out;
    }

    uint32_t* task_directory = task->page_directory->directory_entry;
    uint32_t old_entry = paging_get(task_directory, tmp);
    map_virtual_address_to_physical_address(task->page_directory, tmp, tmp, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    set_current_page_directory(task->page_directory);
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
    kernel_free_alloc(tmp);
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
    set_current_task(CURRENT_TASK_);
    return 0;
}

// Function to switch the page directory to a specified task's page directory
int task_page_task(struct Task* task)
{
    user_registers();
    set_current_page_directory(task->page_directory);
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


static void remove_task_from_list(struct Task* task)
{
    // If the task is not the first task, update the previous task's 'next' pointer
    if (task->prev)
    {
        task->prev->next = task->next;  // Point the previous task's 'next' to the current task's 'next', bypassing the current task
    }

    // Check if the task to remove is the front task in the list
    if (task == TASK_FRONT_)
    {
        TASK_FRONT_ = task->next;  // Update the front task pointer to the next task in the list
    }

    // Check if the task to remove is the tail task in the list
    if (task == TASK_TAIL_)
    {
        TASK_TAIL_ = task->prev;  // Update the tail task pointer to the previous task in the list
    }

    // Check if the task to remove is the currently active task
    if (task == CURRENT_TASK_)
    {
        CURRENT_TASK_ = task_get_next();  // Update the current task pointer to the next available task
    }
}

int task_free(struct Task* task)
{
    paging_free_4gb(task->page_directory);
    remove_task_from_list(task);

    kernel_free_alloc(task);
    return 0;
}

// Function to start the first task
void run_first_task()
{
    if (!CURRENT_TASK_)
    {
        panic("No current task exists!\n");
    }

    set_current_task(TASK_FRONT_);
    task_return(&TASK_FRONT_->registers);
}

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

struct Task* task_new(struct Process* process)
{
    int res = 0;
    struct Task* newTask = kernel_zeroed_alloc(sizeof(struct Task));
    if (!newTask)
    {
        res = -ENOMEM;
        goto out;
    }

    res = task_init(newTask, process);
    if (res != ALL_OK)
    {
        goto out;
    }

    if (TASK_FRONT_ == 0)
    {
        TASK_FRONT_ = newTask;
        TASK_TAIL_ = newTask;
        CURRENT_TASK_ = newTask;
        goto out;
    }

    // add newTask to queue
    {
        TASK_TAIL_->next = newTask;
        newTask->prev = TASK_TAIL_;
        TASK_TAIL_ = newTask;
    }

out:
    if (ISERR(res))
    {
        task_free(newTask);
        return ERROR(res);
    }

    return newTask;
}