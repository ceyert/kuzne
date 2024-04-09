#include "string.h"
#include "syscalls.h"

struct CommandArgument* kuzne_parse_command(const char* command, int max)
{
    struct CommandArgument* root_command = 0;

    char scommand[1025];

    if (max >= (int)sizeof(scommand))
    {
        return 0;
    }

    strncpy(scommand, command, sizeof(scommand));
    char* token = strtok(scommand, " ");
    if (!token)
    {
        goto out;
    }

    root_command = kuzne_syscall_malloc(sizeof(struct CommandArgument));

    if (!root_command)
    {
        goto out;
    }

    strncpy(root_command->argument, token, sizeof(root_command->argument));
    root_command->next = 0;

    struct CommandArgument* current = root_command;
    token = strtok(NULL, " ");
    while (token != 0)
    {
        struct CommandArgument* new_command = kuzne_syscall_malloc(sizeof(struct CommandArgument));
        if (!new_command)
        {
            break;
        }

        strncpy(new_command->argument, token, sizeof(new_command->argument));
        new_command->next = 0x00;
        current->next = new_command;
        current = new_command;
        token = strtok(NULL, " ");
    }
out:
    return root_command;
}

int kuzne_getkeyblock()
{
    int val = 0;
    do
    {
        val = kuzne_syscall_getkey();
    } while (val == 0);
    return val;
}

void kuzne_terminal_readline(char* out, int max, bool output_while_typing)
{
    int i = 0;
    for (i = 0; i < max - 1; i++)
    {
        char key = kuzne_getkeyblock();

        // Carriage return means we have read the line
        if (key == 13)
        {
            break;
        }

        if (output_while_typing)
        {
            kuzne_syscall_putchar(key);
        }

        // Backspace
        if (key == 0x08 && i >= 1)
        {
            out[i - 1] = 0x00;
            // -2 because we will +1 when we go to the continue
            i -= 2;
            continue;
        }

        out[i] = key;
    }

    // Add the null terminator
    out[i] = 0x00;
}

int kuzne_system_run(const char* command)
{
    char buf[1024];
    strncpy(buf, command, sizeof(buf));
    struct CommandArgument* root_command_argument = kuzne_parse_command(buf, sizeof(buf));
    if (!root_command_argument)
    {
        return -1;
    }

    return kuzne_syscall_system(root_command_argument);
}