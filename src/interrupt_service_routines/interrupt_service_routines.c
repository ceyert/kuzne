/**
 * @file interrupt_service_routines.c
 * @brief Implementation of the ISR 80h command registration.
 *
 * Contains the implementation of isr80h_register_commands, which registers all the
 * system command handlers with the ISR 80h system call interface. Each system command
 * is associated with a specific function that implements the command's functionality.
 */

#include "interrupt_service_routines.h"
#include "interrupt_descriptor_table/Idt.h"
#include "Misc_isr.h"
#include "Io_isr.h"
#include "Heap_isr.h"
#include "Process_isr.h"

void isr80h_register_commands() 
{
    isr80h_register_command(SYSTEM_COMMAND0_SUM, isr80h_command0_sum); ///< Register handler for summing two numbers.

    isr80h_register_command(SYSTEM_COMMAND1_PRINT, isr80h_command1_print); ///< Register handler for printing to the console.
    
    isr80h_register_command(SYSTEM_COMMAND2_GETKEY, isr80h_command2_getkey); ///< Register handler for getting a key press.
    
    isr80h_register_command(SYSTEM_COMMAND3_PUTCHAR, isr80h_command3_putchar); ///< Register handler for outputting a character.
    
    isr80h_register_command(SYSTEM_COMMAND4_MALLOC, isr80h_command4_malloc); ///< Register handler for memory allocation.
    
    isr80h_register_command(SYSTEM_COMMAND5_FREE, isr80h_command5_free); ///< Register handler for freeing allocated memory.
    
    isr80h_register_command(SYSTEM_COMMAND6_PROCESS_LOAD_START, isr80h_command6_process_load_start); ///< Register handler for process loading.
    
    isr80h_register_command(SYSTEM_COMMAND7_INVOKE_SYSTEM_COMMAND, isr80h_command7_invoke_system_command); ///< Register handler for invoking system commands.
    
    isr80h_register_command(SYSTEM_COMMAND8_GET_PROGRAM_ARGUMENTS, isr80h_command8_get_program_arguments); ///< Register handler for getting program arguments.
    
    isr80h_register_command(SYSTEM_COMMAND9_EXIT, isr80h_command9_exit); ///< Register handler for program exit.
}