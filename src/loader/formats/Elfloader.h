#ifndef ELFLOADER_H
#define ELFLOADER_H

#include <stdint.h>
#include <stddef.h>

#include "Elf.h"
#include "Config.h"

struct ElfFile {
    char filename[MAX_PATH_SIZE];

    int inMemorySize;

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

extern int elf_load(const char *filename, struct ElfFile **file_out);

extern void elf_close(struct ElfFile *file);

extern void *elf_virtual_base(struct ElfFile *file);

extern void *elf_virtual_end(struct ElfFile *file);

extern void *elf_phys_base(struct ElfFile *file);

extern void *elf_phys_end(struct ElfFile *file);

extern struct ElfHeader *elf_header(struct ElfFile *file);

extern struct Elf32Shdr *elf_sheader(struct ElfHeader *header);

extern void *elf_memory(struct ElfFile *file);

extern struct Elf32Phdr *elf_pheader(struct ElfHeader *header);

extern struct Elf32Phdr *elf_program_header(struct ElfHeader *header, int index);

extern struct Elf32Shdr *elf_section(struct ElfHeader *header, int index);

extern void *elf_phdr_phys_address(struct ElfFile *file, struct Elf32Phdr *phdr);

#endif