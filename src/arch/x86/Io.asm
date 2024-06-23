section .asm


; Low-level input/output operations between the CPU and peripheral devices. 
; These operations use the in and out instructions for reading from and writing to device ports. 

; /usr/include/x86_64-linux-gnu/sys/io.h

global insb
global insw
global outb
global outw

; insb: Reads a byte from a specified I/O port into AL register.
insb:
    push ebp                ; Save the base pointer
    mov ebp, esp            ; Establish a stack frame

    xor eax, eax            ; Clear EAX to ensure AL will be clean
    mov edx, [ebp+8]        ; Load the port address into EDX
    in al, dx               ; Read a byte from the port into AL

    pop ebp                 ; Restore the base pointer
    ret                     

; insw: Reads a word (2 bytes) from a specified I/O port into AX register.
insw:
    push ebp                ; Save the base pointer
    mov ebp, esp            ; Establish a stack frame

    xor eax, eax            ; Clear EAX to ensure AX will be clean
    mov edx, [ebp+8]        ; Load the port address into EDX
    in ax, dx               ; Read a word from the port into AX

    pop ebp                 ; Restore the base pointer
    ret                     

; outb: Writes a byte to a specified I/O port.
outb:
    push ebp                ; Save the base pointer
    mov ebp, esp            ; Establish a stack frame

    mov eax, [ebp+12]       ; Move the byte to be written into EAX
    mov edx, [ebp+8]        ; Load the port address into EDX
    out dx, al              ; Write the byte in AL to the port

    pop ebp                 ; Restore the base pointer
    ret                     

; outw: Writes a word (2 bytes) to a specified I/O port.
outw:
    push ebp                ; Save the base pointer
    mov ebp, esp            ; Establish a stack frame

    mov eax, [ebp+12]       ; Move the word to be written into EAX
    mov edx, [ebp+8]        ; Load the port address into EDX
    out dx, ax              ; Write the word in AX to the port

    pop ebp                 ; Restore the base pointer
    ret                     