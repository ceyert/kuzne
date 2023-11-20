#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>

#include "Task.h"
#include "Config.h"

#define PROCESS_FILETYPE_ELF 0
#define PROCESS_FILETYPE_BINARY 1

typedef unsigned char process_filetype_t;

struct ProcessAllocation {
    void *ptr;
    size_t size;
};

struct CommandArgument {
    char argument[512];
    struct CommandArgument *next;
};

struct ProcessArguments {
    int argc;
    char **argv;
};

struct Process {
    // The process id
    uint16_t id;

    char filename[MAX_PATH_SIZE];

    // The main process task
    struct Task *task;

    // The memory (malloc) allocations of the process
    struct ProcessAllocation allocations[MAX_PROGRAM_ALLOCATIONS];

    process_filetype_t filetype;

    union {
        // The physical pointer to the process memory.
        void *processBaseAddr;
        struct ElfFile *elf_file;
    };


    // The physical pointer to the stack memory
    void *stackPtr;

    // The size of the data pointed to by "ptr"
    uint32_t size;

    struct KeyboardBuffer {
        char buffer[KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;

    // The arguments of the process.
    struct ProcessArguments processArguments;
};

extern int process_switch(struct Process *process);

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

#endif