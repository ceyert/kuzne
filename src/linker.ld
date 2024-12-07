ENTRY(_start)                       /* Specify entry point */
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 1M;                         /* Start at 1MB (0x00100000) */
    .text : ALIGN(4096)             /* Kernel text section */
    {
        _start = .;                 /* Entry point */
        *(.text)                    /* General text code */
    }

    .rodata : ALIGN(4096)           /* Read-only data */
    {
        *(.rodata)
    }

    .data : ALIGN(4096)             /* Writable data */
    {
        *(.data)
    }

    .bss : ALIGN(4096)              /* Uninitialized data */
    {
        *(COMMON)
        *(.bss)
    }
}