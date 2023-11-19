#ifndef PEACHOS_H
#define PEACHOS_H

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


void print(const char *filename);

int peachos_getkey();

void *peachos_malloc(size_t size);

void peachos_free(void *ptr);

void peachos_putchar(char c);

int peachos_getkeyblock();

void peachos_terminal_readline(char *out, int max, bool output_while_typing);

void peachos_process_load_start(const char *filename);

struct CommandArgument *peachos_parse_command(const char *command, int max);

void peachos_process_get_arguments(struct ProcessArguments *arguments);

int peachos_system(struct CommandArgument *arguments);

int peachos_system_run(const char *command);

void peachos_exit();

#endif