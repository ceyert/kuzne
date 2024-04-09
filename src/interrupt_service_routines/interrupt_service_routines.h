/**
 * @file interrupt_service_routines.h
 * @brief Definitions for system commands and registration function for ISR 80h.
 *
 * This header defines an enumeration of system commands that can be invoked through the
 * ISR 80h system call interface. It also declares the function used to register all the
 * system command handlers.
 */

#ifndef ISR80H_H
#define ISR80H_H

/**
 * @enum SystemCommands
 * @brief Enumeration of system commands accessible via the ISR 80h system call interface.
 *
 * Defines identifiers for various system commands that the kernel can execute in response
 * to system calls made by user programs. These commands include basic I/O operations,
 * memory management, and process control.
 */
enum SystemCommands
{
    SYSTEM_COMMAND0_SUM = 0,                    ///< Sums two numbers.
    SYSTEM_COMMAND1_PRINT = 1,                  ///< Prints a string to the console.
    SYSTEM_COMMAND2_GETKEY = 2,                 ///< Gets the next key press.
    SYSTEM_COMMAND3_PUTCHAR = 3,                ///< Outputs a character to the console.
    SYSTEM_COMMAND4_MALLOC = 4,                 ///< Allocates memory on the heap.
    SYSTEM_COMMAND5_FREE = 5,                   ///< Frees allocated memory.
    SYSTEM_COMMAND6_PROCESS_LOAD_START = 6,     ///< Starts loading a process.
    SYSTEM_COMMAND7_INVOKE_SYSTEM_COMMAND = 7,  ///< Invokes another system command.
    SYSTEM_COMMAND8_GET_PROGRAM_ARGUMENTS = 8,  ///< Retrieves program arguments.
    SYSTEM_COMMAND9_EXIT = 9                    ///< Exits the current program.
};

/**
 * @brief Registers handlers for the system commands.
 *
 * Calls the isr80h_register_command function for each system command to associate
 * the command with its handler function. This setup is necessary for the ISR 80h
 * system call interface to function correctly.
 */
extern void isr80h_register_commands();

#endif  // ISR80H_H
