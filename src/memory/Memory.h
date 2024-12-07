/**
 * @file memory.h
 * @brief Memory and string manipulation functions for the kernel.
 * 
 * This header file provides function declarations for memory operations
 * (like `memset`, `memcpy`) and string manipulation utilities.
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Fills a block of memory with a specified value.
 * 
 * @param ptr Pointer to the starting address of the memory block.
 * @param c The value to set (interpreted as an unsigned char).
 * @param size Number of bytes to set.
 * @return Pointer to the memory block.
 */
extern void *memset(void *ptr, int c, size_t size);

/**
 * @brief Compares two memory blocks.
 * 
 * @param s1 Pointer to the first memory block.
 * @param s2 Pointer to the second memory block.
 * @param count Number of bytes to compare.
 * @return An integer less than, equal to, or greater than zero if s1 is found,
 *         respectively, to be less than, to match, or to be greater than s2.
 */
extern int memcmp(void *s1, void *s2, int count);

/**
 * @brief Copies a block of memory from one location to another.
 * 
 * @param dest Pointer to the destination memory block.
 * @param src Pointer to the source memory block.
 * @param len Number of bytes to copy.
 * @return Pointer to the destination memory block.
 */
extern void *memcpy(void *dest, void *src, int len);

/**
 * @brief Calculates the length of a null-terminated string.
 * 
 * @param ptr Pointer to the string.
 * @return The length of the string, excluding the null terminator.
 */
extern int strlen(const char *ptr);

/**
 * @brief Calculates the length of a string up to a maximum length.
 * 
 * @param ptr Pointer to the string.
 * @param max Maximum number of characters to inspect.
 * @return The length of the string, or max if the string is longer.
 */
extern int strnlen(const char *ptr, int max);

/**
 * @brief Checks if a character is a digit.
 * 
 * @param c The character to check.
 * @return True if the character is a digit, otherwise false.
 */
extern bool isdigit(char c);

/**
 * @brief Converts a character digit to its numeric equivalent.
 * 
 * @param c The character digit to convert.
 * @return The numeric value of the digit.
 */
extern int tonumericdigit(char c);

/**
 * @brief Copies a null-terminated string to a destination buffer.
 * 
 * @param dest Pointer to the destination buffer.
 * @param src Pointer to the source string.
 * @return Pointer to the destination buffer.
 */
extern char *strcpy(char *dest, const char *src);

/**
 * @brief Copies up to a specified number of characters from one string to another.
 * 
 * @param dest Pointer to the destination buffer.
 * @param src Pointer to the source string.
 * @param count Maximum number of characters to copy.
 * @return Pointer to the destination buffer.
 */
extern char *strncpy(char *dest, const char *src, int count);

/**
 * @brief Compares up to n characters of two strings.
 * 
 * @param str1 Pointer to the first string.
 * @param str2 Pointer to the second string.
 * @param n Number of characters to compare.
 * @return An integer indicating the comparison result.
 */
extern int strncmp(const char *str1, const char *str2, int n);

/**
 * @brief Compares up to n characters of two strings case-insensitively.
 * 
 * @param s1 Pointer to the first string.
 * @param s2 Pointer to the second string.
 * @param n Number of characters to compare.
 * @return An integer indicating the comparison result.
 */
extern int istrncmp(const char *s1, const char *s2, int n);

/**
 * @brief Calculates the length of a string up to a maximum length or terminator.
 * 
 * @param str Pointer to the string.
 * @param max Maximum number of characters to inspect.
 * @param terminator The character to terminate the search.
 * @return The length of the string up to the terminator or max.
 */
extern int strnlen_terminator(const char *str, int max, char terminator);

/**
 * @brief Tokenizes a string using specified delimiters.
 * 
 * @param str Pointer to the string to tokenize. Pass NULL to continue tokenizing.
 * @param delimiters Null-terminated string of delimiters.
 * @return Pointer to the next token, or NULL if there are no more tokens.
 */
extern char *strtok(char *str, const char *delimiters);

/**
 * @brief Extracts a substring from a source string.
 * 
 * @param src Pointer to the source string.
 * @param start Start index of the substring (0-based).
 * @param length Length of the substring to extract.
 * @param dest Pointer to the destination buffer to store the substring.
 * @return Pointer to the destination buffer.
 */
extern char* substr(const char* src, int start, int length, char* dest);

/**
 * @brief Converts an uppercase character to lowercase.
 * 
 * @param s1 The character to convert.
 * @return The lowercase equivalent, or the same character if it is not uppercase.
 */
extern char tolower(char s1);

/**
 * @brief Converts an integer to a null-terminated string in decimal format.
 * 
 * @param i The integer to convert.
 * @return Pointer to the resulting string.
 */
extern char *itoa(int i);

/**
 * @brief Converts an integer to a null-terminated string in hexadecimal format.
 * 
 * @param i The integer to convert.
 * @return Pointer to the resulting hexadecimal string.
 */
extern char *itoa_hex(int i);

/**
 * @brief Converts a pointer address to a null-terminated hexadecimal string.
 * 
 * @param ptr The pointer address to convert.
 * @return Pointer to the resulting hexadecimal string.
 */
extern char *ptr_to_hex(const unsigned long ptr);

#endif /* MEMORY_H */