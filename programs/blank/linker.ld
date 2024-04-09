# entry point symbol for the executable.
ENTRY(_start)

# executable to ELF 32-bit for Intel 386 architecture.
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    # Start the location counter at virtual address 0x400000, 
    # which is a common base address for executables.
    . = 0x400000;

    # Define the .text section (executable code), aligning it to a 4KB boundary.
    # This helps ensure that the code section starts on a page boundary,
    .text : ALIGN(4096)
    {
        # Include all .text sections from input files.
        *(.text)
    }

    # Define the .asm section, which may contain additional assembly code,
    # aligning it to a 4KB boundary as well for consistency and efficiency.
    .asm : ALIGN(4096)
    {
        # Include all .asm sections from input files.
        *(.asm)
    }
    
    # Define the .rodata section for read-only data (like constants and strings),
    # aligning it to a 4KB boundary to prevent mixing with executable code
    # and to enhance protection against modification.
    .rodata : ALIGN(4096)
    {
        # Include all .rodata sections from input files.
        *(.rodata)
    }

    # Define the .data section for initialized global and static variables,
    # aligning it to a 4KB boundary to separate it from read-only data.
    .data : ALIGN(4096)
    {
        # Include all .data sections from input files.
        *(.data)
    }

    # Define the .bss section for uninitialized (zero-initialized) data,
    # aligning it to a 4KB boundary. This section does not take up space in the
    # executable file but is allocated in memory at runtime.
    .bss : ALIGN(4096)
    {
        # Include common blocks and all .bss sections from input files.
        *(COMMON)
        *(.bss)
    }
}
