section .asm

; External references to handlers defined elsewhere
extern int21h_handler              ; External handler for interrupt 0x21
extern no_interrupt_handler         ; External handler for unused interrupts
extern isr80h_handler               ; External system call handler (interrupt 0x80)
extern interrupt_handler            ; General interrupt handler

; Global symbols for access from other modules
global idt_load                     ; Function to load the IDT
global no_interrupt                 ; Default no-op interrupt handler
global enable_interrupts            ; Enable interrupts function
global disable_interrupts           ; Disable interrupts function
global isr80h_wrapper               ; Wrapper for system call handling
global interrupt_pointer_table      ; Table of interrupt pointers

; -----------------------------------------------------------------------------
; Enable Interrupts
; -----------------------------------------------------------------------------
enable_interrupts:
    sti                             ; Set the interrupt flag (enable interrupts)
    ret                             ; Return to the caller

; -----------------------------------------------------------------------------
; Disable Interrupts
; -----------------------------------------------------------------------------
disable_interrupts:
    cli                             ; Clear the interrupt flag (disable interrupts)
    ret                             ; Return to the caller

; -----------------------------------------------------------------------------
; Load the IDT
; -----------------------------------------------------------------------------
idt_load:
    push ebp                        ; Save the base pointer
    mov ebp, esp                    ; Set up the stack frame
    mov ebx, [ebp+8]                ; Get the address of the IDT descriptor from the first argument
    lidt [ebx]                      ; Load the IDT using the `lidt` instruction
    pop ebp                         ; Restore the base pointer
    ret                             ; Return to the caller

; -----------------------------------------------------------------------------
; Default No-Interrupt Handler
; -----------------------------------------------------------------------------
no_interrupt:
    pushad                          ; Save all general-purpose registers
    call no_interrupt_handler       ; Call the external handler for unused interrupts
    popad                           ; Restore the general-purpose registers
    iret                            ; Return from the interrupt

; -----------------------------------------------------------------------------
; Macro for Defining Interrupt Service Routines
; -----------------------------------------------------------------------------
%macro interrupt 1
    global int%1                    ; Declare the interrupt globally
    int%1:
        pushad                      ; Save all general-purpose registers
        push esp                    ; Push the pointer to the interrupt frame
        push dword %1               ; Push the interrupt number
        call interrupt_handler      ; Call the general interrupt handler
        add esp, 8                  ; Adjust the stack to remove the pushed parameters
        popad                       ; Restore the general-purpose registers
        iret                        ; Return from the interrupt
%endmacro

; Generate 512 Interrupt Service Routines
%assign i 0
%rep 512
    interrupt i                     ; Use the `interrupt` macro to define each ISR
%assign i i+1
%endrep

; -----------------------------------------------------------------------------
; System Call Wrapper (Interrupt 0x80)
; -----------------------------------------------------------------------------
isr80h_wrapper:
    pushad                          ; Save all general-purpose registers

    ; Push the stack pointer to provide the interrupt frame to the handler
    push esp

    ; Push the EAX register (contains the system call ID) for the system call handler
    push eax
    call isr80h_handler             ; Call the system call handler
    mov dword [tmp_res], eax        ; Save the result in `tmp_res`
    add esp, 8                      ; Clean up the stack (remove handler arguments)

    ; Restore the general-purpose registers
    popad
    mov eax, [tmp_res]              ; Retrieve the result into EAX
    iretd                           ; Return from the interrupt with the saved context

; -----------------------------------------------------------------------------
; Data Section
; -----------------------------------------------------------------------------
section .data
tmp_res: dd 0                       ; Temporary storage for the result of `isr80h_handler`

; -----------------------------------------------------------------------------
; Interrupt Pointer Table
; -----------------------------------------------------------------------------
%macro interrupt_array_entry 1
    dd int%1                        ; Define an entry in the interrupt pointer table
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i         ; Populate the table with the address of each ISR
%assign i i+1
%endrep