#include "Config.h"
#include "Kernel.h"
#include "Process.h"
#include "Status.h"
#include "fs/File.h"
#include "loader/Elfloader.h"
#include "memory/Memory.h"
#include "malloc/Kheap.h"
#include "paging/Paging.h"
#include "process/Task.h"
#include "vga/Vga.h"

// The current process that is running
struct Process* CURRENT_PROCESS_ = 0;

static struct Process* PROCESS_BUFFER_[MAX_PROCESSES] = {};

// Initializes a process by setting all its fields to zero
static void process_init(struct Process* process)
{
    memset(process, 0, sizeof(struct Process));
}

// Returns the currently running process
struct Process* process_current()
{
    return CURRENT_PROCESS_;
}

// Retrieves a process by its ID, or returns NULL if the ID is invalid or out of range
struct Process* process_get(int process_id)
{
    if (process_id < 0 || process_id >= MAX_PROCESSES)
    {
        return NULL;
    }

    return PROCESS_BUFFER_[process_id];
}

// Switches the current process to the specified process
int set_current_process(struct Process* process)
{
    CURRENT_PROCESS_ = process;
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
    void* ptr = kernel_zeroed_alloc(size);
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
        kernel_free_alloc(ptr);
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
    kernel_free_alloc(process->processBaseAddr);
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


void get_next_not_free_process()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (PROCESS_BUFFER_[i] != 0)
        {
            set_current_process(PROCESS_BUFFER_[i]);
            return;
        }
    }

    panic("No process switch to!\n");
}


static void process_unlink(struct Process* process)
{
    PROCESS_BUFFER_[process->processId] = 0x00; // free current process 

    if (CURRENT_PROCESS_ == process)
    {
        get_next_not_free_process();
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
    kernel_free_alloc(process->stackPtr);

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
    kernel_free_alloc(ptr);
}

// Loads a binary file into a process
static int load_binary_file(const char* filename, struct Process* process)
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

    program_data_ptr = kernel_zeroed_alloc(stat.filesize);
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
            kernel_free_alloc(program_data_ptr);
        }
    }
    fclose(fd);
    return res;
}

// Maps a binary file into the process's address space
int process_map_binary(struct Process* process)
{
    int res = 0;
    paging_map_to(process->task->page_directory, (void*)USER_PROCESS_VIRTUAL_BASE_ADDRESS_NON_ELF, process->processBaseAddr,
                  paging_align_address(process->processBaseAddr + process->size),
                  PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    return res;
}


/**
 * 
 * Processes the PT_LOAD program header to map sections into memory.
 * The PT_LOAD program header type is crucial for defining executable and data segments
 * that need to be loaded into memory for a program to run. This function is responsible
 * for interpreting the PT_LOAD header from an ELF file and mapping the corresponding
 * sections (.text, .data, .rodata, and .bss) into the virtual address space of the process.
 * 
 * Maps ELF file segments into the virtual address space of a process.
 *
 * This function takes ELF file, then maps each segment of the ELF file physical address into 
 * process's virtual address space according to the segment headers. 
 * 
 * Mapping is necessary to execute the program contained in the ELF
 * file by making its code and data accessible in the process's memory space.
 *
 * The function utilizes the paging mechanism to map virtual addresses to physical
 * addresses, respecting the permissions and requirements defined in the ELF program headers.
 */
static int process_map_loadable_elf_sections(struct Process* process)
{
    int res = 0;

    struct ElfFile* elf_file = process->elfFile;

    struct ElfHeader* elf_header_ptr = elf_header(elf_file);

    struct Elf32Phdr* program_headers = elf_pheader(elf_header_ptr);

    for (int i = 0; i < elf_header_ptr->e_phnum; i++)
    {
        struct Elf32Phdr* program_header = &program_headers[i];

        void* program_header_physical_address = elf_program_header_physical_address(elf_file, program_header);

        int page_flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;

        if (program_header->p_flags & PF_W)
        {
            page_flags |= PAGING_IS_WRITEABLE;
        }

        res = paging_map_to(process->task->page_directory, paging_align_to_lower_page((void*)program_header->p_vaddr),
                            paging_align_to_lower_page(program_header_physical_address),
                            paging_align_address(program_header_physical_address + program_header->p_memsz), page_flags);
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
            res = process_map_loadable_elf_sections(process);
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

    // Maps physical stack memory address with predefined stack virtual memory address.
    paging_map_to(process->task->page_directory, 
                (void*)USER_PROCESS_STACK_VIRTUAL_ADDRESS_BASE, process->stackPtr,
                paging_align_address(process->stackPtr + USER_PROCESS_STACK_SIZE),
                PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);

    logAddress("stack: ", (unsigned long)process->stackPtr);
    logAddress("process stack base (pre-defined): ", USER_PROCESS_STACK_VIRTUAL_ADDRESS_BASE);

out:
    return res;
}

// Loads an ELF file into a process
static int load_elf_file(const char* filename, struct Process* process)
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
    logAddress("physical_base_address: ", (unsigned long)process->elfFile->physical_base_address);
    logAddress("physical_end_address: ", (unsigned long)process->elfFile->physical_end_address);
    logAddress("virtual_base_address: ", (unsigned long)process->elfFile->virtual_base_address);
    logAddress("virtual_end_address: ", (unsigned long)process->elfFile->virtual_end_address);
out:
    return res;
}

// Loads either an ELF file or a binary file into a process
static int load_program_file(const char* filename, struct Process* process)
{
    int res = 0;
    res = load_elf_file(filename, process);
    if (res == -EINFORMAT)
    {
        res = load_binary_file(filename, process);
    }

    return res;
}


int get_free_process_idx_from_buffer()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (PROCESS_BUFFER_[i] == 0) 
        {
            return i;
        }
    }

    return -EISTKN;
}

// Loads a process for a specific slot in the process array
int process_load_for_slot(const char* filename, struct Process** process, int process_slot)
{
    int res = 0;
    struct Task* task = 0;
    struct Process* new_process_;
    void* program_stack_ptr = 0;

    if (process_get(process_slot) != 0)
    {
        res = -EISTKN;
        goto out;
    }

    new_process_ = kernel_zeroed_alloc(sizeof(struct Process));
    if (!new_process_)
    {
        res = -ENOMEM;
        goto out;
    }

    process_init(new_process_);
    res = load_program_file(filename, new_process_);
    if (res < 0)
    {
        goto out;
    }

    program_stack_ptr = kernel_zeroed_alloc(USER_PROCESS_STACK_SIZE);
    if (!program_stack_ptr)
    {
        res = -ENOMEM;
        goto out;
    }

    strncpy(new_process_->filename, filename, sizeof(new_process_->filename));
    new_process_->stackPtr = program_stack_ptr;
    new_process_->processId = process_slot;

    logAddress("stack allocated: ", (unsigned long)new_process_->stackPtr);

    // Create a new task
    task = task_new(new_process_);

    if (ERROR_I(task) == 0)
    {
        res = ERROR_I(task);
        goto out;
    }

    new_process_->task = task;

    res = process_map_memory(new_process_);
    if (res < 0)
    {
        goto out;
    }

    *process = new_process_;

    // Add the process to process buffer
    PROCESS_BUFFER_[process_slot] = new_process_;

out:
    if (ISERR(res))
    {
        if (new_process_ && new_process_->task)
        {
            // Free the process data
            task_free(new_process_->task);
        }
    }
    return res;
}

// Loads a process from a file and allocates a slot for it
int process_load(const char* filename, struct Process** process)
{
    int res = 0;
    int process_slot = get_free_process_idx_from_buffer();
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
        set_current_process(*process);
    }

    return res;
}