#ifndef KUZNE_SYSCALLS_H
#define KUZNE_SYSCALLS_H

#include <stddef.h>
#include <stdbool.h>


struct CommandArgument {
    char argument[512];
    struct CommandArgument *next;
};

struct ProcessArguments {
    int argc;
    char **argv;
};


extern void print(const char *filename);

extern int kuzne_getkey();

extern void *kuzne_malloc(size_t size);

extern void kuzne_free(void *ptr);

extern void kuzne_putchar(char c);

extern int kuzne_getkeyblock();

extern void kuzne_terminal_readline(char *out, int max, bool output_while_typing);

extern void kuzne_process_load_start(const char *filename);

extern struct CommandArgument *kuzne_parse_command(const char *command, int max);

extern void kuzne_process_get_arguments(struct ProcessArguments *arguments);

extern int kuzne_system(struct CommandArgument *arguments);

extern int kuzne_system_run(const char *command);

extern void kuzne_exit();

#endif