[BITS 32]

global _start
extern c_start
extern kuzne_exit

section .asm

_start:
    call c_start
    call kuzne_exit
    ret