#ifndef ELF_H
#define ELF_H

#include <stddef.h>
#include <stdint.h>

#define PF_X 0x01
#define PF_W 0x02
#define PF_R 0x04

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOPROC 12
#define SHT_HIPROC 13
#define SHT_LOUSER 14
#define SHT_HIUSER 15

#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4

#define EI_NIDENT 16
#define EI_CLASS 4
#define EI_DATA 5

#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2

#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define SHN_UNDEF 0

typedef uint16_t elf32_half_t;
typedef uint32_t elf32_word_t;
typedef int32_t elf32_sword_t;
typedef uint32_t elf32_addr_t;
typedef int32_t elf32_off_t;

struct Elf32Phdr
{
    elf32_word_t p_type;
    elf32_off_t p_offset;
    elf32_addr_t p_vaddr;
    elf32_addr_t p_paddr;
    elf32_word_t p_filesz;
    elf32_word_t p_memsz;
    elf32_word_t p_flags;
    elf32_word_t p_align;
} __attribute__((packed));

struct Elf32Shdr
{
    elf32_word_t sh_name;
    elf32_word_t sh_type;
    elf32_word_t sh_flags;
    elf32_addr_t sh_addr;
    elf32_off_t sh_offset;
    elf32_word_t sh_size;
    elf32_word_t sh_link;
    elf32_word_t sh_info;
    elf32_word_t sh_addralign;
    elf32_word_t sh_entsize;
} __attribute__((packed));

struct ElfHeader
{
    unsigned char e_ident[EI_NIDENT];
    elf32_half_t e_type;
    elf32_half_t e_machine;
    elf32_word_t e_version;
    elf32_addr_t e_entry;
    elf32_off_t e_phoff;
    elf32_off_t e_shoff;
    elf32_word_t e_flags;
    elf32_half_t e_ehsize;
    elf32_half_t e_phentsize;
    elf32_half_t e_phnum;
    elf32_half_t e_shentsize;
    elf32_half_t e_shnum;
    elf32_half_t e_shstrndx;
} __attribute__((packed));

struct Elf32Dyn
{
    elf32_sword_t d_tag;
    union
    {
        elf32_word_t d_val;
        elf32_addr_t d_ptr;
    } d_un;

} __attribute__((packed));

struct Elf32Sym
{
    elf32_word_t st_name;
    elf32_addr_t st_value;
    elf32_word_t st_size;
    unsigned char st_info;
    unsigned char st_other;
    elf32_half_t st_shndx;
} __attribute__((packed));

extern void* elf_get_entry_ptr(struct ElfHeader* elf_header);

extern uint32_t elf_get_entry(struct ElfHeader* elf_header);

#endif