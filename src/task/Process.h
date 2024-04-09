/**
 * @file Process.h
 * @brief Process management and manipulation functions.
 *
 * Defines the structure of a process in the system, including its memory allocations, arguments,
 * and state. Provides functions for creating, switching, and managing processes.
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>

#include "Task.h"
#include "Config.h"

// Process file type definitions
#define PROCESS_FILETYPE_ELF 0 ///< ELF format.
#define PROCESS_FILETYPE_BINARY 1 ///< Binary format, such as a.out.

typedef unsigned char process_filetype_t; ///< Type for process file format.

/**
 * @struct ProcessAllocation
 * @brief Represents a single memory allocation within a process.
 */
struct ProcessAllocation {
    void *ptr;  ///< Pointer to the allocated memory.
    size_t size; ///< Size of the allocated memory.
};

/**
 * @struct CommandArgument
 * @brief Linked list of command line arguments.
 */
struct CommandArgument {
    char argument[512]; ///< Single command line argument.
    struct CommandArgument *next; ///< Pointer to the next argument in the list.
};

/**
 * @struct ProcessArguments
 * @brief Contains process arguments in a format suitable for passing to a program.
 */
struct ProcessArguments {
    int argc;   ///< Argument count.
    char **argv; ///< Argument values.
};

/**
 * @struct Process
 * @brief Represents a process in the system.
 */
struct Process {
    uint16_t processId; ///< The process ID.

    char filename[MAX_PATH_SIZE]; ///< Filename associated with the process.

    struct Task *task; ///< Main process task.

    struct ProcessAllocation allocations[MAX_PROGRAM_ALLOCATIONS]; ///< Memory allocations of the process.

    process_filetype_t fileType; ///< File type of the process executable.

    union {
        void *processBaseAddr; ///< Base address of the process memory for binary files.
        struct ElfFile *elfFile; ///< Pointer to ELF file structure if process is an ELF.
    };

    void *stackPtr; ///< Pointer to the process's stack memory.

    uint32_t size; ///< Size of the process memory.

    struct KeyboardBuffer { ///< Keyboard circular buffer for the process.
        char buffer[KEYBOARD_BUFFER_SIZE]; ///< Actual buffer.
        int tail; ///< Tail index for the circular buffer.
        int head; ///< Head index for the circular buffer.
    } keyboard;

    struct ProcessArguments processArguments; ///< Command line arguments for the process.
};

extern int set_current_process(struct Process *process);

extern int process_load_switch(const char *filename, struct Process **process);

extern int process_load(const char *filename, struct Process **process);

extern int process_load_for_slot(const char *filename, struct Process **process, int process_slot);

extern struct Process *process_current();

extern struct Process *process_get(int process_id);

extern void *process_malloc(struct Process *process, size_t size);

extern void process_free(struct Process *process, void *ptr);

extern void process_get_arguments(struct Process *process, int *argc, char ***argv);

extern int process_inject_arguments(struct Process *process, struct CommandArgument *root_argument);

extern int process_terminate(struct Process *process);

#endif // PROCESS_H