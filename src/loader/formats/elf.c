#include "elf.h"

void *elf_get_entry_ptr(struct ElfHeader *elf_header) {
    return (void *) elf_header->e_entry;
}

uint32_t elf_get_entry(struct ElfHeader *elf_header) {
    return elf_header->e_entry;
}