#include "Config.h"
#include "Kernel.h"
#include "Process_isr.h"
#include "Status.h"
#include "memory/Memory.h"
#include "process/Process.h"
#include "process/Task.h"

void* isr80h_command6_process_load_start(struct InterruptFrame* frame)
{
    void* filename_user_ptr = task_get_stack_item(task_current(), 0);
    char filename[MAX_PATH_SIZE];
    int res = copy_string_from_task(task_current(), filename_user_ptr, filename, sizeof(filename));
    if (res < 0)
    {
        goto out;
    }

    char path[MAX_PATH_SIZE];
    strcpy(path, "0:/");
    strcpy(path + 3, filename);

    struct Process* process = 0;
    res = process_load_switch(path, &process);
    if (res < 0)
    {
        goto out;
    }

    set_current_task(process->task);
    task_return(&process->task->registers);

out:
    return 0;
}

void* isr80h_command7_invoke_system_command(struct InterruptFrame* frame)
{
    struct CommandArgument* arguments =
        task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));
    if (!arguments || strlen(arguments[0].argument) == 0)
    {
        return ERROR(-EINVARG);
    }

    struct CommandArgument* root_command_argument = &arguments[0];
    const char* program_name = root_command_argument->argument;

    char path[MAX_PATH_SIZE];
    strcpy(path, "0:/");
    strncpy(path + 3, program_name, sizeof(path));

    struct Process* process = 0;
    int res = process_load_switch(path, &process);
    if (res < 0)
    {
        return ERROR(res);
    }

    res = process_inject_arguments(process, root_command_argument);
    if (res < 0)
    {
        return ERROR(res);
    }

    set_current_task(process->task);
    task_return(&process->task->registers);

    return 0;
}

void* isr80h_command8_get_program_arguments(struct InterruptFrame* frame)
{
    struct Process* process = task_current()->process;
    struct ProcessArguments* arguments =
        task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));

    process_get_arguments(process, &arguments->argc, &arguments->argv);
    return 0;
}

void* isr80h_command9_exit(struct InterruptFrame* frame)
{
    struct Process* process = task_current()->process;

    process_terminate(process);
    
    run_next_task();
    
    return 0;
}