/**
 * @file ELFLoader.h
 * @brief Loader for ELF (Executable and Linkable Format) files.
 *
 * Provides functionality to load ELF files into memory, access their headers and sections,
 * and manage their memory allocation. It is designed to work with ELF files for loading and
 * running executables or shared libraries in a custom operating system or runtime environment.
 */

#ifndef ELFLOADER_H
#define ELFLOADER_H

#include <stdint.h>
#include <stddef.h>

#include "Elf.h"
#include "Config.h"

/**
 * @struct ElfFile
 * @brief Represents an ELF file loaded into memory.
 *
 * This structure holds information about an ELF file that has been loaded into memory,
 * including its filename, memory addresses, and sizes.
 */
struct ElfFile {
    char filename[MAX_PATH_SIZE]; ///< The filename of the ELF file.

    int inMemorySize; ///< Size of the ELF file loaded in memory.

    void *elf_memory; ///< Physical memory address where the ELF file is loaded.

    void *virtual_base_address; ///< Virtual base address of the binary.

    void *virtual_end_address; ///< Ending virtual address of the binary.

    void *physical_base_address; ///< Physical base address of the binary.

    void *physical_end_address; ///< Physical end address of the binary.
};

/**
 * @brief Loads an ELF file into memory.
 *
 * @param filename Path to the ELF file to be loaded.
 * @param file_out Pointer to where the loaded ELF file structure will be stored.
 * @return int Returns 0 on success, negative values on error.
 */
extern int elf_load(const char *filename, struct ElfFile **file_out);

/**
 * @brief Closes and frees resources associated with an ELF file.
 *
 * @param file Pointer to the ElfFile structure to be freed.
 */
extern void elf_close(struct ElfFile *file);

/**
 * @brief Gets the virtual base address of the ELF file.
 *
 * @param file Pointer to the ElfFile structure.
 * @return void* Virtual base address of the ELF file.
 */
extern void *elf_virtual_base(struct ElfFile *file);

/**
 * @brief Gets the ending virtual address of the ELF file.
 *
 * @param file Pointer to the ElfFile structure.
 * @return void* Ending virtual address of the ELF file.
 */
extern void *elf_virtual_end(struct ElfFile *file);

/**
 * @brief Gets the physical base address of the ELF file.
 *
 * @param file Pointer to the ElfFile structure.
 * @return void* Physical base address of the ELF file.
 */
extern void *elf_phys_base(struct ElfFile *file);

/**
 * @brief Gets the physical end address of the ELF file.
 *
 * @param file Pointer to the ElfFile structure.
 * @return void* Physical end address of the ELF file.
 */
extern void *elf_phys_end(struct ElfFile *file);

/**
 * @brief Gets the ELF header of the file.
 *
 * @param file Pointer to the ElfFile structure.
 * @return struct ElfHeader* Pointer to the ELF header.
 */
extern struct ElfHeader *elf_header(struct ElfFile *file);

/**
 * @brief Gets the section header of the ELF file.
 *
 * @param header Pointer to the ELF header.
 * @return struct Elf32Shdr* Pointer to the first section header.
 */
extern struct Elf32Shdr *elf_sheader(struct ElfHeader *header);

/**
 * @brief Gets the memory address where the ELF file is loaded.
 *
 * @param file Pointer to the ElfFile structure.
 * @return void* Memory address of the loaded ELF file.
 */
extern void *elf_memory(struct ElfFile *file);

/**
 * @brief Gets the program header of the ELF file.
 *
 * @param header Pointer to the ELF header.
 * @return struct Elf32Phdr* Pointer to the first program header.
 */
extern struct Elf32Phdr *elf_pheader(struct ElfHeader *header);

/**
 * @brief Gets a specific program header of the ELF file.
 *
 * @param header Pointer to the ELF header.
 * @param index Index of the program header to retrieve.
 * @return struct Elf32Phdr* Pointer to the specified program header.
 */
extern struct Elf32Phdr *elf_program_header(struct ElfHeader *header, int index);

/**
 * @brief Gets a specific section of the ELF file.
 *
 * @param header Pointer to the ELF header.
 * @param index Index of the section to retrieve.
 * @return struct Elf32Shdr* Pointer to the specified section header.
 */
extern struct Elf32Shdr *elf_section(struct ElfHeader *header, int index);

/**
 * @brief Calculates the physical address of a segment described by a program header.
 *
 * @param file Pointer to the ElfFile structure.
 * @param phdr Pointer to the Elf32Phdr (program header).
 * @return void* Physical address of the segment.
 */
extern void *elf_program_header_physical_address(struct ElfFile *file, struct Elf32Phdr *phdr);

#endif // ELFLOADER_H