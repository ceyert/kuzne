#ifndef ELFLOADER_H
#define ELFLOADER_H

#include <stdint.h>
#include <stddef.h>

#include "elf.h"
#include "config.h"

struct ElfFile {
    char filename[PEACHOS_MAX_PATH];

    int in_memory_size;

    /**
     * The physical memory address that this elf file is loaded at
     */
    void *elf_memory;

    /**
     * The virtual base address of this binary
     */
    void *virtual_base_address;

    /**
     * The ending virtual address
     */
    void *virtual_end_address;

    /**
     * The physical base address of this binary
     */
    void *physical_base_address;

    /**
     * The physical end address of this bunary
     */
    void *physical_end_address;


};

int elf_load(const char *filename, struct ElfFile **file_out);

void elf_close(struct ElfFile *file);

void *elf_virtual_base(struct ElfFile *file);

void *elf_virtual_end(struct ElfFile *file);

void *elf_phys_base(struct ElfFile *file);

void *elf_phys_end(struct ElfFile *file);

struct ElfHeader *elf_header(struct ElfFile *file);

struct Elf32Shdr *elf_sheader(struct ElfHeader *header);

void *elf_memory(struct ElfFile *file);

struct Elf32Phdr *elf_pheader(struct ElfHeader *header);

struct Elf32Phdr *elf_program_header(struct ElfHeader *header, int index);

struct Elf32Shdr *elf_section(struct ElfHeader *header, int index);

void *elf_phdr_phys_address(struct ElfFile *file, struct Elf32Phdr *phdr);

#endif