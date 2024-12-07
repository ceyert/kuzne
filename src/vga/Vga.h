/**
 * @file Vga.h
 * @brief Terminal-related definitions and functions for the kernel.
 * 
 * This header file provides constants and function declarations for interacting
 * with the terminal, including initialization, text output, and logging.
 */

#ifndef TERMINAL_H
#define TERMINAL_H

/**
 * @brief Base address of the VGA text mode buffer.
 */
#define VGA_BASE_ADDR 0xB8000

/**
 * @brief Number of columns in the VGA text mode.
 */
#define VGA_COL 80

/**
 * @brief Number of rows in the VGA text mode.
 */
#define VGA_ROW 20

/**
 * @brief Initializes the terminal for use.
 * 
 * This function sets up the terminal by clearing the screen and preparing
 * it for text output.
 */
extern void terminal_initialize();

/**
 * @brief Prints a string to the terminal.
 * 
 * @param str The null-terminated string to print.
 */
extern void print(const char *str);

/**
 * @brief Writes a single character to the terminal with the specified color.
 * 
 * @param c The character to write.
 * @param colour The color attribute for the character.
 */
extern void terminal_writechar(char c, char colour);

/**
 * @brief Triggers a kernel panic with a message.
 * 
 * This function halts the kernel and displays the provided message, 
 * indicating a critical error.
 * 
 * @param msg The panic message to display.
 */
extern void panic(const char *msg);

/**
 * @brief Logs a message to the terminal.
 * 
 * @param msg The null-terminated log message.
 */
extern void log(const char *msg);

/**
 * @brief Logs a message and an associated memory address to the terminal.
 * 
 * @param msg The null-terminated message.
 * @param addr The memory address to display alongside the message.
 */
extern void logAddress(const char *msg, const unsigned long addr);

#endif /* TERMINAL_H */
