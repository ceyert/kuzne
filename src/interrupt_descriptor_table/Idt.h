/**
 * @file IDT.h
 * @brief Definitions and functions for setting up and managing the Interrupt Descriptor Table (IDT).
 *
 * This header file contains definitions for structures related to the Interrupt Descriptor Table (IDT),
 * as well as function prototypes for initializing the IDT, handling interrupts, and managing interrupt
 * service routines (ISRs) and callbacks.
 */

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// Forward declaration of InterruptFrame for use in function pointers.
struct InterruptFrame;

/**
 * @brief Type definition for system call commands.
 *
 * Function pointer type for system call commands, allowing the kernel to call
 * user-defined functions in response to system calls made via interrupt 0x80.
 *
 * @param frame Pointer to an interrupt frame structure containing the state before the interrupt.
 * @return Pointer to the result of the system call command.
 */
typedef void *(*ISR80H_COMMAND)(struct InterruptFrame *frame);

/**
 * @brief Type definition for interrupt callback functions.
 *
 * Function pointer type for interrupt callback functions, enabling the registration
 * of custom callback functions for hardware and software interrupts.
 */
typedef void(*INTERRUPT_CALLBACK_FUNCTION)();

/**
 * @brief Structure defining an IDT entry.
 *
 * Represents an entry in the Interrupt Descriptor Table (IDT), describing
 * the properties of a single interrupt gate.
 */
struct IdtDesc {
    uint16_t offset_1;      ///< Offset bits 0 - 15 of the interrupt handler
    uint16_t selector;      ///< Code segment selector in GDT
    uint8_t zero;           ///< Unused, set to zero
    uint8_t type_attr;      ///< Type and attributes of the descriptor
    uint16_t offset_2;      ///< Offset bits 16-31 of the interrupt handler
} __attribute__((packed));

/**
 * @brief Structure defining the IDTR register.
 *
 * Describes the size and location of the Interrupt Descriptor Table (IDT),
 * used when loading the IDT register (IDTR).
 */
struct IdtrDesc {
    uint16_t limit;         ///< Limit of the IDT (size - 1)
    uint32_t base;          ///< Base address of the IDT
} __attribute__((packed));

/**
 * @brief Structure representing the state before an interrupt.
 *
 * Contains the registers saved by the CPU automatically upon an interrupt
 * and by the interrupt stub. Used in the context of interrupt service routines.
 */
struct InterruptFrame {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;       ///< Instruction pointer before the interrupt.
    uint32_t cs;       ///< Code segment before the interrupt.
    uint32_t flags;    ///< CPU flags before the interrupt.
    uint32_t esp;      ///< Stack pointer before the interrupt.
    uint32_t ss;       ///< Stack segment before the interrupt.
} __attribute__((packed));

/**
 * @brief Initializes the Interrupt Descriptor Table (IDT).
 *
 * Sets up the IDT with predefined entries and loads it into the CPU.
 */
extern void idt_init();

/**
 * @brief Enables hardware interrupts.
 *
 * Unmasks hardware interrupts by clearing the IF flag in the EFLAGS register.
 */
extern void enable_interrupts();

/**
 * @brief Disables hardware interrupts.
 *
 * Masks hardware interrupts by setting the IF flag in the EFLAGS register.
 */
extern void disable_interrupts();

/**
 * @brief Registers a system call command with the kernel.
 *
 * Allows the registration of a custom system call handler for system calls made via interrupt 0x80.
 *
 * @param command_id The identifier for the system call command.
 * @param command The system call command function to register.
 */
extern void isr80h_register_command(int command_id, ISR80H_COMMAND command);

/**
 * @brief Registers an interrupt callback function.
 *
 * Allows the registration of a custom callback function for a specific interrupt vector.
 *
 * @param interrupt The interrupt vector for which the callback is being registered.
 * @param interrupt_callback The callback function to be called when the interrupt occurs.
 * @return int Status code of the registration process (success or error).
 */
extern int idt_register_interrupt_callback(int interrupt, INTERRUPT_CALLBACK_FUNCTION interrupt_callback);

#endif // IDT_H
