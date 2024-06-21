#include "shell.h"
#include "stdio.h"
#include "memory.h"
#include "stdlib.h"
#include "syscalls.h"

int main(int argc, char** argv)
{
    println("Shell v1.0.0");

    int RESULT = 0;

    while (1)
    {
        print("$ ");

        char buffer[1024];

        kuzne_terminal_readline(buffer, sizeof(buffer), true);

        if (strncmp(buffer, "exit", 4) == 0)
        {
            print("\n");
            break;
        }

        if (strncmp(buffer, "run ", 4) == 0)
        {
            print("\n");
            char temp[1024];
            RESULT = kuzne_system_run(substr(buffer, 4, strlen(buffer) - 4, temp));
        }

        print("\n");
    }

    return RESULT; // return to kernel
}