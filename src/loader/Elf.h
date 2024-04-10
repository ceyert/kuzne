/**
 * @file ELF.h
 * @brief Definitions for parsing and working with ELF (Executable and Linkable Format) files.
 *
 * This header provides structures, type definitions, and function prototypes for working with
 * ELF files, which are commonly used for executables and shared libraries in Unix-like operating systems.
 */

#ifndef ELF_H
#define ELF_H

#include <stddef.h>
#include <stdint.h>

// ELF Program Header Permission Flags
#define PF_X 0x01 ///< Execute permission
#define PF_W 0x02 ///< Write permission
#define PF_R 0x04 ///< Read permission

// ELF Program Header Types
#define PT_NULL    0 ///< Unused entry
#define PT_LOAD    1 ///< Loadable segment
#define PT_DYNAMIC 2 ///< Dynamic linking information
#define PT_INTERP  3 ///< Program interpreter path name
#define PT_NOTE    4 ///< Note sections
#define PT_SHLIB   5 ///< Reserved
#define PT_PHDR    6 ///< Program header table itself

// ELF Section Header Types
#define SHT_NULL     0  ///< Unused section header
#define SHT_PROGBITS 1  ///< Program-specific information
#define SHT_SYMTAB   2  ///< Symbol table
#define SHT_STRTAB   3  ///< String table
#define SHT_RELA     4  ///< Relocation entries with addends
#define SHT_HASH     5  ///< Symbol hash table
#define SHT_DYNAMIC  6  ///< Dynamic linking information
#define SHT_NOTE     7  ///< Notes
#define SHT_NOBITS   8  ///< Occupies no space in the file
#define SHT_REL      9  ///< Relocation entries without addends
#define SHT_SHLIB    10 ///< Reserved
#define SHT_DYNSYM   11 ///< Dynamic linker symbol table
#define SHT_LOPROC   12 ///< Processor-specific
#define SHT_HIPROC   13 ///< Processor-specific
#define SHT_LOUSER   14 ///< Lower bound for application-specific
#define SHT_HIUSER   15 ///< Upper bound for application-specific

// ELF File Types
#define ET_NONE 0 ///< No file type
#define ET_REL  1 ///< Relocatable file
#define ET_EXEC 2 ///< Executable file
#define ET_DYN  3 ///< Shared object file
#define ET_CORE 4 ///< Core file

// ELF Identification Indexes
#define EI_NIDENT 16 ///< Size of e_ident[]
#define EI_CLASS  4  ///< File class byte index
#define EI_DATA   5  ///< Data encoding byte index

// ELF Class Codes
#define ELFCLASSNONE 0 ///< Invalid class
#define ELFCLASS32   1 ///< 32-bit objects
#define ELFCLASS64   2 ///< 64-bit objects

// ELF Data Encoding
#define ELFDATANONE 0 ///< Invalid data encoding
#define ELFDATA2LSB 1 ///< 2's complement, little endian
#define ELFDATA2MSB 2 ///< 2's complement, big endian

// Special Section Indexes
#define SHN_UNDEF 0 ///< Undefined/missing section

// Type definitions for ELF components
typedef uint16_t elf32_half_t;  ///< ELF 32-bit half-word
typedef uint32_t elf32_word_t;  ///< ELF 32-bit word
typedef int32_t  elf32_sword_t; ///< ELF 32-bit signed word
typedef uint32_t elf32_addr_t;  ///< ELF 32-bit address
typedef int32_t  elf32_off_t;   ///< ELF 32-bit file offset

/**
 * @struct Elf32Phdr
 * @brief ELF 32-bit Program Header.
 *
 * Describes a single segment or other information the system needs to prepare the program for execution.
 */
struct Elf32Phdr
{
    elf32_word_t p_type;   ///< Segment type
    elf32_off_t p_offset;  ///< Segment file offset
    elf32_addr_t p_vaddr;  ///< Segment virtual address
    elf32_addr_t p_paddr;  ///< Segment physical address (unused)
    elf32_word_t p_filesz; ///< Number of bytes in the file image of the segment
    elf32_word_t p_memsz;  ///< Number of bytes in the memory image of the segment
    elf32_word_t p_flags;  ///< Segment flags
    elf32_word_t p_align;  ///< Segment alignment
} __attribute__((packed));

/**
 * @struct Elf32Shdr
 * @brief ELF 32-bit Section Header.
 *
 * Describes a single section in an ELF file.
 */
struct Elf32Shdr
{
    elf32_word_t sh_name;      ///< Section name (index into the section header string table)
    elf32_word_t sh_type;      ///< Section type
    elf32_word_t sh_flags;     ///< Section flags
    elf32_addr_t sh_addr;      ///< Section virtual address at execution
    elf32_off_t sh_offset;     ///< Section file offset
    elf32_word_t sh_size;      ///< Section size in bytes
    elf32_word_t sh_link;      ///< Link to another section
    elf32_word_t sh_info;      ///< Additional section information
    elf32_word_t sh_addralign; ///< Section alignment
    elf32_word_t sh_entsize;   ///< Entry size if section holds table
} __attribute__((packed));

/**
 * @struct ElfHeader
 * @brief ELF File Header.
 *
 * Contains information about the file format, sizes, and structure.
 */
struct ElfHeader
{
    unsigned char e_ident[EI_NIDENT]; ///< Magic number and other info
    elf32_half_t e_type;              ///< Object file type
    elf32_half_t e_machine;           ///< Architecture
    elf32_word_t e_version;           ///< Object file version
    elf32_addr_t e_entry;             ///< Entry point virtual address
    elf32_off_t e_phoff;              ///< Program header table file offset
    elf32_off_t e_shoff;              ///< Section header table file offset
    elf32_word_t e_flags;             ///< Processor-specific flags
    elf32_half_t e_ehsize;            ///< ELF header size in bytes
    elf32_half_t e_phentsize;         ///< Program header table entry size
    elf32_half_t e_phnum;             ///< Program header table entry count
    elf32_half_t e_shentsize;         ///< Section header table entry size
    elf32_half_t e_shnum;             ///< Section header table entry count
    elf32_half_t e_shstrndx;          ///< Section header string table index
} __attribute__((packed));

/**
 * @struct Elf32Dyn
 * @brief ELF 32-bit Dynamic Table Entry.
 *
 * Used in dynamic linking information.
 */
struct Elf32Dyn
{
    elf32_sword_t d_tag; ///< Dynamic entry type
    union
    {
        elf32_word_t d_val; ///< Integer value
        elf32_addr_t d_ptr; ///< Address value
    } d_un;
} __attribute__((packed));

/**
 * @struct Elf32Sym
 * @brief ELF 32-bit Symbol Table Entry.
 *
 * Describes a single symbol within an ELF file.
 */
struct Elf32Sym
{
    elf32_word_t st_name;  ///< Symbol name (string tbl index)
    elf32_addr_t st_value; ///< Symbol value
    elf32_word_t st_size;  ///< Symbol size
    unsigned char st_info; ///< Symbol type and binding
    unsigned char st_other; ///< Symbol visibility
    elf32_half_t st_shndx; ///< Section index
} __attribute__((packed));

/**
 * @brief Retrieves the entry point address from an ELF header.
 *
 * @param elf_header Pointer to an ELF header.
 * @return Pointer to the entry point function.
 */
extern void* elf_get_entry_ptr(struct ElfHeader* elf_header);

/**
 * @brief Gets the entry point address from an ELF header.
 *
 * @param elf_header Pointer to an ELF header.
 * @return The entry point address.
 */
extern uint32_t elf_get_entry(struct ElfHeader* elf_header);

#endif // ELF_H