#include "Config.h"
#include "Elfloader.h"
#include "Kernel.h"
#include "Status.h"
#include "fs/File.h"
#include "malloc/Kheap.h"
#include "memory/Memory.h"
#include "paging/Paging.h"
#include "vga/Vga.h"
#include <stdbool.h>

const char elf_signature[] = {0x7f, 'E', 'L', 'F'};

static bool elf_valid_signature(void* buffer)
{
    return memcmp(buffer, (void*)elf_signature, sizeof(elf_signature)) == 0;
}

static bool elf_valid_class(struct ElfHeader* header)
{
    // We only support 32 bit binaries.
    return header->e_ident[EI_CLASS] == ELFCLASSNONE || header->e_ident[EI_CLASS] == ELFCLASS32;
}

static bool elf_valid_encoding(struct ElfHeader* header)
{
    return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}

static bool elf_is_executable(struct ElfHeader* header)
{
    return header->e_type == ET_EXEC && header->e_entry >= USER_PROCESS_VIRTUAL_BASE_ADDRESS_NON_ELF;
}

static bool elf_has_program_header(struct ElfHeader* header)
{
    return header->e_phoff != 0;
}

void* elf_memory(struct ElfFile* file)
{
    return file->elf_memory;
}

struct ElfHeader* elf_header(struct ElfFile* file)
{
    return file->elf_memory;
}

struct Elf32Shdr* elf_sheader(struct ElfHeader* header)
{
    return (struct Elf32Shdr*)((int)header + header->e_shoff);
}

struct Elf32Phdr* elf_pheader(struct ElfHeader* header)
{
    if (header->e_phoff == 0)
    {
        return 0;
    }

    return (struct Elf32Phdr*)((int)header + header->e_phoff);
}

struct Elf32Phdr* elf_program_header(struct ElfHeader* header, int index)
{
    return &elf_pheader(header)[index];
}

struct Elf32Shdr* elf_section(struct ElfHeader* header, int index)
{
    return &elf_sheader(header)[index];
}

void* elf_program_header_physical_address(struct ElfFile* file, struct Elf32Phdr* phdr)
{
    return elf_memory(file) + phdr->p_offset;
}

char* elf_str_table(struct ElfHeader* header)
{
    return (char*)header + elf_section(header, header->e_shstrndx)->sh_offset;
}

void* elf_virtual_base(struct ElfFile* file)
{
    return file->virtual_base_address;
}

void* elf_virtual_end(struct ElfFile* file)
{
    return file->virtual_end_address;
}

void* elf_phys_base(struct ElfFile* file)
{
    return file->physical_base_address;
}

void* elf_phys_end(struct ElfFile* file)
{
    return file->physical_end_address;
}

void elf_close(struct ElfFile* file)
{
    if (!file) return;

    kernel_free_alloc(file->elf_memory);
    kernel_free_alloc(file);
}

int elf_validate_loaded(struct ElfHeader* header)
{
    return (elf_valid_signature(header) && elf_valid_class(header) && elf_valid_encoding(header)
            && elf_has_program_header(header))
               ? ALL_OK
               : -EINFORMAT;
}

int elf_process_phdr_pt_load(struct ElfFile* elf_file, struct Elf32Phdr* phdr)
{
    if (elf_file->virtual_base_address >= (void*)phdr->p_vaddr || elf_file->virtual_base_address == 0x00)
    {
        elf_file->virtual_base_address = (void*)phdr->p_vaddr;
        elf_file->physical_base_address = elf_memory(elf_file) + phdr->p_offset;
    }

    unsigned int end_virtual_address = phdr->p_vaddr + phdr->p_filesz;

    if (elf_file->virtual_end_address <= (void*)(end_virtual_address) || elf_file->virtual_end_address == 0x00)
    {
        elf_file->virtual_end_address = (void*)end_virtual_address;
        elf_file->physical_end_address = elf_memory(elf_file) + phdr->p_offset + phdr->p_filesz;
    }
    return 0;
}

int elf_process_pheader(struct ElfFile* elf_file, struct Elf32Phdr* phdr)
{
    int res = 0;
    switch (phdr->p_type)
    {
        // All loadable sections are part of PT_LOAD segment.
        case PT_LOAD:
            // load sections into memory (.TEXT, .DATA, .BSS, .RODATA)
            res = elf_process_phdr_pt_load(elf_file, phdr);
            break;
    }
    return res;
}

int elf_process_pheaders(struct ElfFile* elf_file)
{
    int res = 0;
    struct ElfHeader* header = elf_header(elf_file);
    for (int i = 0; i < header->e_phnum; i++)
    {
        struct Elf32Phdr* phdr = elf_program_header(header, i);
        res = elf_process_pheader(elf_file, phdr);
        if (res < 0)
        {
            break;
        }
    }
    return res;
}

int elf_process_loaded(struct ElfFile* elf_file)
{
    int res = 0;
    struct ElfHeader* header = elf_header(elf_file);
    res = elf_validate_loaded(header);
    if (res < 0)
    {
        goto out;
    }

    res = elf_process_pheaders(elf_file);
    if (res < 0)
    {
        goto out;
    }

out:
    return res;
}

int elf_load(const char* filename, struct ElfFile** file_out)
{
    struct ElfFile* elf_file = kernel_zeroed_alloc(sizeof(struct ElfFile));
    int fd = 0;
    int res = fopen(filename, "r");
    if (res <= 0)
    {
        res = -EIO;
        goto out;
    }

    fd = res;
    struct FileStat stat;
    res = fstat(fd, &stat);
    if (res < 0)
    {
        goto out;
    }

    elf_file->elf_memory = kernel_zeroed_alloc(stat.filesize);

    logAddress("elf_memory : ", (unsigned long)elf_file->elf_memory);

    res = fread(elf_file->elf_memory, stat.filesize, 1, fd);
    if (res < 0)
    {
        goto out;
    }

    res = elf_process_loaded(elf_file);
    if (res < 0)
    {
        goto out;
    }

    *file_out = elf_file;
out:
    fclose(fd);
    return res;
}