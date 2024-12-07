[BITS 32]                  ; Code is written in 32-bit protected mode

global _start              ; Entry point of the kernel
global kernel_registers    ; Global label for kernel register setup
extern kernel_main         ; External reference to the kernel's main function

CODE_SEG equ 0x08          ; Code segment selector in the GDT
DATA_SEG equ 0x10          ; Data segment selector in the GDT

; -----------------------------------------------------------------------------
; Kernel Entry Point
; -----------------------------------------------------------------------------
_start:
    ; Set all segment registers to the data segment
    mov ax, DATA_SEG       ; Load the data segment selector
    mov ds, ax             ; Set DS (Data Segment)
    mov es, ax             ; Set ES (Extra Segment)
    mov fs, ax             ; Set FS (Additional Data Segment)
    mov gs, ax             ; Set GS (Additional Data Segment)
    mov ss, ax             ; Set SS (Stack Segment)

    ; Set kernel stack pointer (ESP) and base pointer (EBP)
    ; Stack is initialized at 0x00200000 (2MB offset), which should be well-aligned
    mov ebp, 0x00200000    ; Set the base pointer to 2MB
    mov esp, ebp           ; Set the stack pointer to the same address

    ; Enable the A20 Line (to access memory beyond 1MB)
    in al, 0x92            ; Read the current value of port 0x92
    or al, 2               ; Set the A20 gate bit
    out 0x92, al           ; Write the updated value to port 0x92

    ; Remap the Master PIC (Programmable Interrupt Controller)
    mov al, 00010001b      ; ICW1: Initialize PIC and require ICW4
    out 0x20, al           ; Send initialization command to Master PIC (Command port 0x20)

    mov al, 0x20           ; ICW2: Set the master PIC vector offset to 0x20
    out 0x21, al           ; Send vector offset to Master PIC (Data port 0x21)

    mov al, 00000001b      ; ICW4: Set PIC to 8086/88 (MCS-80/85) mode
    out 0x21, al           ; Send ICW4 to Master PIC

    ; Call the kernel's main function
    call kernel_main       ; Transition to the kernel's main execution

    ; Infinite loop to halt the processor after kernel_main returns
    jmp $                  ; Jump to the current address, creating an infinite loop

; -----------------------------------------------------------------------------
; Kernel Register Setup
; -----------------------------------------------------------------------------
kernel_registers:
    ; Reinitialize all segment registers to the data segment selector (0x10)
    mov ax, DATA_SEG       ; Load the data segment selector
    mov ds, ax             ; Set DS (Data Segment)
    mov es, ax             ; Set ES (Extra Segment)
    mov gs, ax             ; Set GS (Additional Data Segment)
    mov fs, ax             ; Set FS (Additional Data Segment)
    ret                    ; Return to the caller

; -----------------------------------------------------------------------------
; Bootloader Padding
; -----------------------------------------------------------------------------
times 512-($ - $$) db 0    ; Pad the binary to 512 bytes (typical for boot sectors)