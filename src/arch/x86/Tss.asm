section .asm

global tss_load

tss_load:
    push ebp
    mov ebp, esp
    mov ax, [ebp+8]
    ltr ax   ; TSS Segment
    pop ebp
    ret