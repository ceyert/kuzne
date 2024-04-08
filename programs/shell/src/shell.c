#include "shell.h"
#include "stdio.h"
#include "stdlib.h"
#include "syscalls.h"

int main(int argc, char** argv)
{
    print("Shell v1.0.0\n");

    while (1)
    {
        print("> ");

        char buffer[1024];
        kuzne_terminal_readline(buffer, sizeof(buffer), true);

        print("\n");

        kuzne_system_run(buffer);

        print("\n");
    }

    return 0;
}