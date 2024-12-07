section .asm              ; Start of the assembly section
global gdt_load           ; Declare the `gdt_load` function as global for external linkage

gdt_load:
    ; Load the GDT (Global Descriptor Table) using the LGDT instruction
    mov eax, [esp+4]       ; Load the GDT base address from the stack (second parameter)
    mov [gdt_descriptor + 2], eax ; Store the base address into the GDT descriptor
    mov ax, [esp+8]        ; Load the GDT size (first parameter, 2 bytes)
    mov [gdt_descriptor], ax ; Store the size into the GDT descriptor
    lgdt [gdt_descriptor]  ; Load the GDT descriptor using the LGDT instruction
    ret                    ; Return from the function

section .data             ; Start of the data section
gdt_descriptor:
    dw 0x00               ; GDT size (16 bits), initialized to 0
    dd 0x00               ; GDT base address (32 bits), initialized to 0