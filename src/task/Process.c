#include "Config.h"
#include "Kernel.h"
#include "Process.h"
#include "Status.h"
#include "fs/File.h"
#include "loader/formats/Elfloader.h"
#include "memory/Memory.h"
#include "memory/heap/Kheap.h"
#include "memory/paging/Paging.h"
#include "string/String.h"
#include "task/Task.h"
#include "vga/Vga.h"

// The current process that is running
struct Process* current_process = 0;

static struct Process* processes[MAX_PROCESSES] = {};

// Initializes a process by setting all its fields to zero
static void process_init(struct Process* process)
{
    memset(process, 0, sizeof(struct Process));
}

// Returns the currently running process
struct Process* process_current()
{
    return current_process;
}

// Retrieves a process by its ID, or returns NULL if the ID is invalid or out of range
struct Process* process_get(int process_id)
{
    if (process_id < 0 || process_id >= MAX_PROCESSES)
    {
        return NULL;
    }

    return processes[process_id];
}

// Switches the current process to the specified process
int process_switch(struct Process* process)
{
    current_process = process;
    return 0;
}

// Finds a free allocation index for a new memory allocation within a process
static int process_find_free_allocation_index(struct Process* process)
{
    int res = -ENOMEM;
    for (int i = 0; i < MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == 0)
        {
            res = i;
            break;
        }
    }

    return res;
}

// Allocates memory of the specified size for a process, and maps it to the process's page directory
void* process_malloc(struct Process* process, size_t size)
{
    void* ptr = kzalloc(size);
    if (!ptr)
    {
        goto out_err;
    }

    int index = process_find_free_allocation_index(process);
    if (index < 0)
    {
        goto out_err;
    }

    int res = paging_map_to(process->task->page_directory, ptr, ptr, paging_align_address(ptr + size),
                            PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if (res < 0)
    {
        goto out_err;
    }

    process->allocations[index].ptr = ptr;
    process->allocations[index].size = size;
    return ptr;

out_err:
    if (ptr)
    {
        kfree(ptr);
    }
    return 0;
}

// Checks if the given pointer is part of the process's memory allocations
static bool process_is_process_pointer(struct Process* process, void* ptr)
{
    for (int i = 0; i < MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == ptr) return true;
    }

    return false;
}

// Removes a memory allocation from a process's list of allocations
static void process_allocation_unjoin(struct Process* process, void* ptr)
{
    for (int i = 0; i < MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == ptr)
        {
            process->allocations[i].ptr = 0x00;
            process->allocations[i].size = 0;
        }
    }
}

// Retrieves a process allocation by its address
static struct ProcessAllocation* process_get_allocation_by_addr(struct Process* process, void* addr)
{
    for (int i = 0; i < MAX_PROGRAM_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == addr) return &process->allocations[i];
    }

    return 0;
}

// Terminates all memory allocations of a process
int process_terminate_allocations(struct Process* process)
{
    for (int i = 0; i < MAX_PROGRAM_ALLOCATIONS; i++)
    {
        process_free(process, process->allocations[i].ptr);
    }

    return 0;
}

// Frees the memory allocated for a process's binary data
int process_free_binary_data(struct Process* process)
{
    kfree(process->processBaseAddr);
    return 0;
}

// Frees the memory allocated for a process's ELF file data
int process_free_elf_data(struct Process* process)
{
    elf_close(process->elfFile);
    return 0;
}

// Frees the memory allocated for a process's program data, depending on its file type
int process_free_program_data(struct Process* process)
{
    int res = 0;
    switch (process->fileType)
    {
        case PROCESS_FILETYPE_BINARY:
            res = process_free_binary_data(process);
            break;

        case PROCESS_FILETYPE_ELF:
            res = process_free_elf_data(process);
            break;

        default:
            res = -EINVARG;
    }
    return res;
}

// Switches to any available process. If no processes are available, triggers a panic
void process_switch_to_any()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i])
        {
            process_switch(processes[i]);
            return;
        }
    }

    panic("No processes to switch too\n");
}

// Removes a process from the process array and switches to another process if necessary
static void process_unlink(struct Process* process)
{
    processes[process->processId] = 0x00;

    if (current_process == process)
    {
        process_switch_to_any();
    }
}

// Terminates a process, freeing all its resources
int process_terminate(struct Process* process)
{
    int res = 0;

    res = process_terminate_allocations(process);
    if (res < 0)
    {
        goto out;
    }

    res = process_free_program_data(process);
    if (res < 0)
    {
        goto out;
    }

    // Free the process stack memory.
    kfree(process->stackPtr);
    // Free the task
    task_free(process->task);
    // Unlink the process from the process array.
    process_unlink(process);

out:
    return res;
}

// Retrieves the arguments passed to a process
void process_get_arguments(struct Process* process, int* argc, char*** argv)
{
    *argc = process->processArguments.argc;
    *argv = process->processArguments.argv;
}

// Counts the number of command arguments
int process_count_command_arguments(struct CommandArgument* root_argument)
{
    struct CommandArgument* current = root_argument;
    int i = 0;
    while (current)
    {
        i++;
        current = current->next;
    }

    return i;
}

// Injects command-line arguments into a process
int process_inject_arguments(struct Process* process, struct CommandArgument* root_argument)
{
    int res = 0;
    struct CommandArgument* current = root_argument;
    int i = 0;
    int argc = process_count_command_arguments(root_argument);
    if (argc == 0)
    {
        res = -EIO;
        goto out;
    }

    char** argv = process_malloc(process, sizeof(const char*) * argc);
    if (!argv)
    {
        res = -ENOMEM;
        goto out;
    }

    while (current)
    {
        char* argument_str = process_malloc(process, sizeof(current->argument));
        if (!argument_str)
        {
            res = -ENOMEM;
            goto out;
        }

        strncpy(argument_str, current->argument, sizeof(current->argument));
        argv[i] = argument_str;
        current = current->next;
        i++;
    }

    process->processArguments.argc = argc;
    process->processArguments.argv = argv;
out:
    return res;
}

// Frees a memory allocation associated with a process
void process_free(struct Process* process, void* ptr)
{
    // Unlink the pages from the process for the given address
    struct ProcessAllocation* allocation = process_get_allocation_by_addr(process, ptr);
    if (!allocation)
    {
        // Oops its not our pointer.
        return;
    }

    int res = paging_map_to(process->task->page_directory, allocation->ptr, allocation->ptr,
                            paging_align_address(allocation->ptr + allocation->size), 0x00);
    if (res < 0)
    {
        return;
    }

    // Unjoin the allocation
    process_allocation_unjoin(process, ptr);

    // We can now free the memory.
    kfree(ptr);
}

// Loads a binary file into a process
static int process_load_binary(const char* filename, struct Process* process)
{
    void* program_data_ptr = 0x00;
    int res = 0;
    int fd = fopen(filename, "r");
    if (!fd)
    {
        res = -EIO;
        goto out;
    }

    struct FileStat stat;
    res = fstat(fd, &stat);
    if (res != ALL_OK)
    {
        goto out;
    }

    program_data_ptr = kzalloc(stat.filesize);
    if (!program_data_ptr)
    {
        res = -ENOMEM;
        goto out;
    }

    if (fread(program_data_ptr, stat.filesize, 1, fd) != 1)
    {
        res = -EIO;
        goto out;
    }

    process->fileType = PROCESS_FILETYPE_BINARY;
    process->processBaseAddr = program_data_ptr;
    process->size = stat.filesize;

out:
    if (res < 0)
    {
        if (program_data_ptr)
        {
            kfree(program_data_ptr);
        }
    }
    fclose(fd);
    return res;
}

// Loads an ELF file into a process
static int process_load_elf(const char* filename, struct Process* process)
{
    int res = 0;
    struct ElfFile* elf_file = 0;
    res = elf_load(filename, &elf_file);
    if (ISERR(res))
    {
        goto out;
    }

    process->fileType = PROCESS_FILETYPE_ELF;
    process->elfFile = elf_file;
out:
    return res;
}

// Loads either an ELF file or a binary file into a process
static int process_load_data(const char* filename, struct Process* process)
{
    int res = 0;
    res = process_load_elf(filename, process);
    if (res == -EINFORMAT)
    {
        res = process_load_binary(filename, process);
    }

    return res;
}

// Maps a binary file into the process's address space
int process_map_binary(struct Process* process)
{
    int res = 0;
    paging_map_to(process->task->page_directory, (void*)PROGRAM_VIRTUAL_ADDRESS, process->processBaseAddr,
                  paging_align_address(process->processBaseAddr + process->size),
                  PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    return res;
}

// Maps an ELF file into the process's address space
static int process_map_elf(struct Process* process)
{
    int res = 0;

    struct ElfFile* elf_file = process->elfFile;
    struct ElfHeader* header = elf_header(elf_file);
    struct Elf32Phdr* phdrs = elf_pheader(header);
    for (int i = 0; i < header->e_phnum; i++)
    {
        struct Elf32Phdr* phdr = &phdrs[i];
        void* phdr_phys_address = elf_phdr_phys_address(elf_file, phdr);
        int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
        if (phdr->p_flags & PF_W)
        {
            flags |= PAGING_IS_WRITEABLE;
        }
        res = paging_map_to(process->task->page_directory, paging_align_to_lower_page((void*)phdr->p_vaddr),
                            paging_align_to_lower_page(phdr_phys_address),
                            paging_align_address(phdr_phys_address + phdr->p_memsz), flags);
        if (ISERR(res))
        {
            break;
        }
    }
    return res;
}

// Maps the memory of a process based on its file type
int process_map_memory(struct Process* process)
{
    int res = 0;

    switch (process->fileType)
    {
        case PROCESS_FILETYPE_ELF:
            res = process_map_elf(process);
            break;

        case PROCESS_FILETYPE_BINARY:
            res = process_map_binary(process);
            break;

        default:
            panic("process_map_memory: Invalid filetype\n");
    }

    if (res < 0)
    {
        goto out;
    }

    // Finally map the stack
    paging_map_to(process->task->page_directory, (void*)PROGRAM_STACK_VIRTUAL_ADDRESS_END, process->stackPtr,
                  paging_align_address(process->stackPtr + USER_PROGRAM_STACK_SIZE),
                  PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
out:
    return res;
}

// Finds a free slot in the process array
int process_get_free_slot()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i] == 0) return i;
    }

    return -EISTKN;
}

// Loads a process from a file and allocates a slot for it
int process_load(const char* filename, struct Process** process)
{
    int res = 0;
    int process_slot = process_get_free_slot();
    if (process_slot < 0)
    {
        res = -EISTKN;
        goto out;
    }

    res = process_load_for_slot(filename, process, process_slot);
out:
    return res;
}

// Loads a process and switches to it
int process_load_switch(const char* filename, struct Process** process)
{
    int res = process_load(filename, process);
    if (res == 0)
    {
        process_switch(*process);
    }

    return res;
}

// Loads a process for a specific slot in the process array
int process_load_for_slot(const char* filename, struct Process** process, int process_slot)
{
    int res = 0;
    struct Task* task = 0;
    struct Process* _process;
    void* program_stack_ptr = 0;

    if (process_get(process_slot) != 0)
    {
        res = -EISTKN;
        goto out;
    }

    _process = kzalloc(sizeof(struct Process));
    if (!_process)
    {
        res = -ENOMEM;
        goto out;
    }

    process_init(_process);
    res = process_load_data(filename, _process);
    if (res < 0)
    {
        goto out;
    }

    program_stack_ptr = kzalloc(USER_PROGRAM_STACK_SIZE);
    if (!program_stack_ptr)
    {
        res = -ENOMEM;
        goto out;
    }

    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->stackPtr = program_stack_ptr;
    _process->processId = process_slot;

    logAddress("process stackPtr allocated: ", (unsigned long)_process->stackPtr);

    // Create a new task
    task = task_new(_process);

    if (ERROR_I(task) == 0)
    {
        res = ERROR_I(task);
        goto out;
    }

    _process->task = task;

    res = process_map_memory(_process);
    if (res < 0)
    {
        goto out;
    }

    *process = _process;

    // Add the process to process buffer
    processes[process_slot] = _process;

out:
    if (ISERR(res))
    {
        if (_process && _process->task)
        {
            task_free(_process->task);
        }

        // Free the process data
    }
    return res;
}