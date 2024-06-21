/**
 * @file ISR80H_Process.h
 * @brief Declarations for ISR 80h process-related system call handlers.
 *
 * This file contains the declarations of system call handlers related to process management
 * that are invoked through the ISR 80h interface. 
 * These include system calls for starting processes, invoking system commands, retrieving program arguments, and exiting processes.
 */

#ifndef ISR80H_PROCESS_H
#define ISR80H_PROCESS_H

// Forward declaration of InterruptFrame for use in function parameters.
struct InterruptFrame;

/**
 * @brief Starts a new process based on the provided program path and arguments.
 *
 * This system call handler initiates the loading and execution of a new process. It expects the program
 * path and arguments to be provided through the interrupt frame or registers.
 *
 * @param frame Pointer to an interrupt frame structure, containing the state before the interrupt and potentially
 *              arguments for the system call.
 * @return Pointer to a process control block (PCB) or similar structure, if successful; NULL otherwise.
 */
extern void *isr80h_command6_process_load_start(struct InterruptFrame *frame);

/**
 * @brief Invokes a specified system command, passing any required arguments.
 *
 * Allows a process to programmatically invoke other system commands, with arguments passed through the interrupt frame.
 * The specific command and arguments are determined by the process' prior setup in registers or memory referenced by the frame.
 *
 * @param frame Pointer to an interrupt frame structure, containing the command to invoke and its arguments.
 * @return Result of the system command execution, specific to the command invoked.
 */
extern void *isr80h_command7_invoke_system_command(struct InterruptFrame *frame);

/**
 * @brief Retrieves the program arguments for the currently executing process.
 *
 * Intended to be called by a process needing to access its command-line arguments. The arguments are
 * returned in a manner determined by the system's calling convention, typically through registers or
 * a pointer in the interrupt frame.
 *
 * @param frame Pointer to an interrupt frame structure, potentially used to pass back the arguments.
 * @return Pointer to the arguments array, structured according to the system's convention.
 */
extern void *isr80h_command8_get_program_arguments(struct InterruptFrame *frame);

/**
 * @brief Exits the currently executing process.
 *
 * This system call handler facilitates the clean termination of a process. It may perform any necessary cleanup,
 * deallocate resources, and adjust the scheduler or process table accordingly.
 *
 * @param frame Pointer to an interrupt frame structure, unused in this context but provided for consistency.
 * @return Typically does not return, as the process is terminated.
 */
extern void *isr80h_command9_exit(struct InterruptFrame *frame);

#endif // ISR80H_PROCESS_H
