/**
 * @file config.h
 * @brief Configuration constants for kernel and system settings.
 * 
 * This header file defines various constants used throughout the kernel and
 * system. These include selectors, interrupt configurations, file system
 * limits, memory management parameters, and more.
 */

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief Kernel code segment selector.
 */
#define KERNEL_CODE_SELECTOR 0x08

/**
 * @brief Kernel data segment selector.
 */
#define KERNEL_DATA_SELECTOR 0x10

/**
 * @brief Total number of interrupts supported by the kernel.
 */
#define TOTAL_INTERRUPTS 512

/**
 * @brief Size of a disk sector in bytes.
 */
#define SECTOR_SIZE 512

/**
 * @brief Maximum number of supported file systems.
 */
#define MAX_FILESYSTEMS 12

/**
 * @brief Maximum number of file descriptors the system can handle at once.
 */
#define MAX_FILE_DESCRIPTORS 512

/**
 * @brief Maximum allowed path length in the file system.
 */
#define MAX_PATH_SIZE 108

/**
 * @brief Total number of GDT (Global Descriptor Table) segments.
 */
#define TOTAL_GDT_SEGMENTS 6

/**
 * @brief Maximum number of program memory allocations per process.
 */
#define MAX_PROGRAM_ALLOCATIONS 1024

/**
 * @brief Maximum number of processes the kernel can manage.
 */
#define MAX_PROCESSES 12

/**
 * @brief User mode data segment selector.
 */
#define USER_DATA_SEGMENT 0x23

/**
 * @brief User mode code segment selector.
 */
#define USER_CODE_SEGMENT 0x1b

/**
 * @brief Maximum number of ISR (Interrupt Service Routine) 0x80 commands.
 */
#define MAX_ISR80H_COMMANDS 1024

/**
 * @brief Size of the keyboard buffer in bytes.
 */
#define KEYBOARD_BUFFER_SIZE 1024

#endif /* CONFIG_H */