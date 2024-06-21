[BITS 32]  ; 32-bit architecture

section .asm

global paging_load_directory  
global enable_paging          

; paging_load_directory: Loads the address of the page directory into the CR3 register.
; This is a necessary step for enabling paging, as CR3 holds the page directory base register (PDBR).
paging_load_directory:
    push ebp                ; Save the old base pointer value on the stack
    mov ebp, esp            ; Create a new stack frame
    mov eax, [ebp+8]        ; Move the first argument (page directory address) into EAX
    mov cr3, eax            ; Load EAX (page directory address) into CR3 register
    pop ebp                 ; Restore the old base pointer value
    ret                     ; Return to the caller, paging directory is now loaded


; enable_paging: Sets the paging bit (bit 31) in the CR0 register to 1, which enables paging.
; CR0 is a control register that controls system behavior. Bit 31 is the PG flag (paging).
enable_paging:
    push ebp                ; Save the old base pointer value on the stack
    mov ebp, esp            ; Create a new stack frame
    mov eax, cr0            ; Move the current value of CR0 register into EAX
    or eax, 0x80000000      ; Set bit 31 of EAX to 1 (enables paging)
    mov cr0, eax            ; Update CR0 with the new value, effectively enabling paging
    pop ebp                 ; Restore the old base pointer value
    ret                     ; Return to the caller, paging is now enabled